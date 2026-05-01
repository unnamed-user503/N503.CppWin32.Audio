#include "Pch.hpp"
#include "Engine.hpp"

// 1. Project Headers
#include "Device/Context.hpp"
#include "Message/Context.hpp"
#include "Message/Dispatcher.hpp"
#include "Message/Queue.hpp"
#include "Processor.hpp"
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <mfapi.h>

// 6. C++ Standard Libraries
#include <memory>
#include <semaphore>
#include <stop_token>
#include <thread>
#include <utility>

namespace N503::Audio
{

    auto Engine::GetInstance() -> Engine&
    {
        static Engine instance{};
        return instance;
    }

    Engine::Engine()
    {
        m_MessageQueue        = std::make_unique<Message::Queue>();
        m_ResourceContainer   = std::make_unique<Resource::Container>();
        m_DiagnosticsReporter = std::make_unique<Diagnostics::Reporter>();
        m_DiagnosticsReporter->AddSink(std::make_unique<Diagnostics::DebugStringSink>());
    }

    Engine::~Engine()
    {
        Stop();
        Wait();
    }

    auto Engine::Start() -> bool
    {
        if (m_IsRunning.load(std::memory_order_acquire))
        {
            return false;
        }

        // スレッド開始同期用シグナル
        std::binary_semaphore signal{ 0 };

        m_AudioThread = std::jthread(
            [this, &signal](std::stop_token stopToken)
            {
                // スレッドに名前を付ける
                ::SetThreadDescription(::GetCurrentThread(), L"N503.CppWin32.Audio");
                //::SetThreadPriority(::GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

                // COMの初期化
                auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);
                // Media Foundation の開始
                ::MFStartup(MF_VERSION, MFSTARTUP_LITE);

                // スレッドのメッセージキューを強制する
                MSG message{};
                ::PeekMessage(&message, NULL, WM_USER, WM_USER, PM_NOREMOVE);

                // エンジンスレッドが起動したので旗を立てる
                m_IsRunning.store(true, std::memory_order_release);
                // エンジンスレッドが起動したので旗を立てる(外部モジュール用)
                m_StartedEvent.SetEvent();

                // スレッドが開始されたのでメインスレッド側のロックを解放する
                signal.release();

                // オーディオスレッドの開始
                try
                {
                    Run(std::move(stopToken));
                }
                CATCH_LOG();

                // Media Foundation の終了
                ::MFShutdown();

                // エンジンスレッドが終了したので旗を下ろす(外部モジュール用)
                m_StartedEvent.ResetEvent();
                // エンジンスレッドが停止したので旗を下す
                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        // スレッドが開始されるのを待つ
        signal.acquire();

        return true;
    }

    auto Engine::Stop() -> bool
    {
        if (!m_IsRunning.load(std::memory_order_acquire))
        {
            return false;
        }

        m_AudioThread.request_stop();

        const auto threadId = ::GetThreadId(m_AudioThread.native_handle());

        if (threadId != 0)
        {
            if (::PostThreadMessageW(threadId, WM_QUIT, 0, 0))
            {
                return true;
            }
        }

        return false;
    }
    
    auto Engine::Wait() -> bool
    {
        if (m_AudioThread.joinable())
        {
            m_AudioThread.join();
        }

        return true;
    }

    auto Engine::Run(const std::stop_token stopToken) -> void
    {
        Message::Dispatcher messageDispatcher;
        m_DeviceContext  = std::make_unique<Device::Context>();
        m_AudioProcessor = std::make_unique<Audio::Processor>();

        auto cleanup = wil::scope_exit(
            [&]
            {
                m_AudioProcessor.reset();
                m_DeviceContext.reset();

                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        auto ProcessMessage = []() -> bool
        {
            MSG message{};
            while (::PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
            {
                if (message.message == WM_QUIT)
                {
                    return false;
                }
                ::TranslateMessage(&message);
                ::DispatchMessageW(&message);
            }
            return true;
        };

        auto wakeupHandles = { m_MessageQueue->GetWakeupEventHandle() };
        auto isAnyActive   = false;

        while (!stopToken.stop_requested())
        {
            const auto count   = static_cast<DWORD>(wakeupHandles.size());
            const auto handles = wakeupHandles.begin();
            const auto timeout = isAnyActive ? 0 : INFINITE;

            auto result = ::MsgWaitForMultipleObjectsEx(count, handles, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result == WAIT_OBJECT_0 + count)
            {
                if (!ProcessMessage())
                {
                    m_AudioProcessor->WaitForAllStop(); // [重要] XAudio2がリソースを参照中なのにリソースを解放してはいけない
                    return;
                }
            }
            else if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + count))
            {
                Message::Context context = {
                    *m_ResourceContainer
                };
                messageDispatcher.Dispatch(*m_MessageQueue, context);
            }

            isAnyActive = m_AudioProcessor->Process();

            m_DiagnosticsReporter->Report();
        }
    }

} // namespace N503::Audio
