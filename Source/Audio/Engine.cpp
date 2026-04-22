#include "Pch.hpp"
#include "Engine.hpp"

// 1. Project Headers
#include "Command/Dispatcher.hpp"
#include "Command/Queue.hpp"
#include "Device/Context.hpp"
#include "Processor.hpp"
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Severity.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <mfapi.h>

// 6. C++ Standard Libraries
#include <chrono>
#include <format>
#include <memory>
#include <semaphore>
#include <stop_token>
#include <thread>
#include <utility>

namespace N503::Audio
{

    auto Engine::Instance() -> Engine&
    {
        static Engine audioEngine{};
        return audioEngine;
    }

    Engine::Engine()
    {
        m_CommandQueue      = std::make_unique<Command::Queue>();
        m_ResourceContainer = std::make_unique<Resource::Container>();
    }

    Engine::~Engine()
    {
        Stop();
        Wait();
    }

    auto Engine::Start() -> void
    {
        if (m_IsRunning.load(std::memory_order_acquire))
        {
            return;
        }

        // スレッド開始同期用シグナル
        std::binary_semaphore signal{ 0 };

        m_AudioThread = std::jthread(
            [this, &signal](std::stop_token stopToken)
            {
                // スレッドに名前を付ける
                ::SetThreadDescription(::GetCurrentThread(), L"N503.CppWin32.Audio");

                // スレッドのメッセージキューを強制する
                MSG message{};
                ::PeekMessage(&message, NULL, WM_USER, WM_USER, PM_NOREMOVE);

                // スレッドが開始されたのでロックを解放する
                signal.release();

                // エンジンスレッドが起動したので旗を立てる
                m_StartedEvent.SetEvent();

                // COMの初期化
                auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);
                ::MFStartup(MF_VERSION, MFSTARTUP_LITE);
                auto mfshutdown = wil::scope_exit([] { ::MFShutdown(); });

                // オーディオスレッドの開始
                this->Run(std::move(stopToken));
            }
        );

        // スレッドが開始されるのを待つ
        signal.acquire();

        // スレッドを開始したのでフラグを立てておく
        m_IsRunning.store(true, std::memory_order_release);
    }

    auto Engine::Stop() -> void
    {
        if (!::PostThreadMessage(::GetThreadId(m_AudioThread.native_handle()), WM_QUIT, 0, 0))
        {
            const Diagnostics::Entry entry{
                .Severity = Diagnostics::Severity::Error,
                .Expected = std::format("PostThreadMessage failed: Reason={}, Handle={}\n", ::GetLastError(), m_AudioThread.native_handle()).data(),
                .Position = 0
            };

            m_DiagnosticsSink.AddEntry(entry);
        }
    }

    auto Engine::Run(const std::stop_token stopToken) -> void
    {
        m_DeviceContext  = std::make_unique<Device::Context>();
        m_AudioProcessor = std::make_unique<Audio::Processor>();

        auto CleanupResources = wil::scope_exit(
            [&]
            {
                m_AudioProcessor.reset();
                m_DeviceContext.reset();

                m_IsRunning.store(false, std::memory_order_release);
            }
        );

        auto OSMessageDispatch = []() -> bool
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

        auto wakeupHandles = { m_CommandQueue->GetWakeupEventHandle() };

        Command::Dispatcher commandDispatcher;
        Diagnostics::Reporter diagnosticsReporter;
        diagnosticsReporter.AddSink(std::make_shared<Diagnostics::ConsoleSink>());

        bool isAnyActive = false;

        while (!stopToken.stop_requested())
        {
            const auto count   = static_cast<DWORD>(wakeupHandles.size());
            const auto handles = wakeupHandles.begin();
            const auto timeout = isAnyActive ? 0 : INFINITE;

            auto result = ::MsgWaitForMultipleObjectsEx(count, handles, timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + count))
            {
                commandDispatcher.Dispatch(*m_CommandQueue);
            }
            else if (result == WAIT_OBJECT_0 + count)
            {
                if (!OSMessageDispatch())
                {
                    m_AudioProcessor->WaitForAllStop(); // [重要] XAudio2がリソースを参照中なのにリソースを解放してはいけない
                    return;
                }
            }

            isAnyActive = m_AudioProcessor->Process();
#ifdef _DEBUG
            diagnosticsReporter.Submit(m_DiagnosticsSink);
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }

#ifdef _DEBUG
        diagnosticsReporter.Submit(m_DiagnosticsSink);
#endif
        diagnosticsReporter.Stop();
        diagnosticsReporter.Wait();
    }

    auto Engine::Wait() -> void
    {
        if (m_AudioThread.joinable())
        {
            m_AudioThread.join();
        }
    }

} // namespace N503::Audio
