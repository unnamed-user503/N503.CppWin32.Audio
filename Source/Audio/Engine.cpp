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
#include <N503/Diagnostics/Reporter.hpp>

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
#include <N503/Diagnostics/Severity.hpp>

namespace N503::Audio
{

    /// @brief
    /// @return
    auto Engine::Instance() -> Engine&
    {
        static Engine audioEngine{};
        return audioEngine;
    }

    /// @brief
    Engine::Engine()
    {
        m_CommandQueue = std::make_unique<Command::Queue>();
        m_ResourceContainer = std::make_unique<Resource::Container>();

        m_DiagnosticsReporter.AddSink(std::make_shared<Diagnostics::ConsoleSink>());
    }

    /// @brief
    Engine::~Engine()
    {
    }

    /// @brief
    /// @return
    auto Engine::Start() -> void
    {
        if (m_IsThreadRunning.load(std::memory_order_acquire))
        {
            return;
        }

        // スレッド開始同期用シグナル
        std::binary_semaphore signal{ 0 };

        m_AudioThread = std::jthread([this, &signal](std::stop_token stopToken)
        {
            // スレッドに名前を付ける
            ::SetThreadDescription(::GetCurrentThread(), L"N503.CppWin32.Thread.Worker.Audio");

            // スレッドのメッセージキューを強制する
            MSG msg;
            ::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

            // スレッドが開始されたのでロックを解放する
            signal.release();

            // COMの初期化
            auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);
            ::MFStartup(MF_VERSION, MFSTARTUP_LITE);
            auto mfshutdown = wil::scope_exit([]{ ::MFShutdown(); });

            // オーディオスレッドの開始
            this->Run(std::move(stopToken));
        });

        // スレッドが開始されるのを待つ
        signal.acquire();

        // スレッドを開始したのでフラグを立てておく
        m_IsThreadRunning.store(true, std::memory_order_release);
    }

    auto Engine::Stop() -> void
    {
        if (!::PostThreadMessage(::GetThreadId(m_AudioThread.native_handle()), WM_QUIT, 0, 0))
        {
            m_DiagnosticsSink.AddEntry({ Diagnostics::Severity::Error, std::format("PostThreadMessage failed: Reason={}, Handle={}\n", ::GetLastError(), m_AudioThread.native_handle()).data() });
        }
    }

    auto Engine::Run(const std::stop_token stopToken) -> void
    {
        m_DeviceContext = std::make_unique<Device::Context>();
        m_AudioProcessor = std::make_unique<Audio::Processor>();

        auto CleanupResources = wil::scope_exit([&]
        {
            m_AudioProcessor.reset();
            m_DeviceContext.reset();

            m_IsThreadRunning.store(false, std::memory_order_release);
        });

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

        auto wakeupHandles =
        {
            m_CommandQueue->GetWakeupEventHandle()
        };

        Command::Dispatcher commandDispatcher;

        bool isAnyActive = false;

        while (!stopToken.stop_requested())
        {
            auto timeout = isAnyActive ? 0 : INFINITE;
            auto result = ::MsgWaitForMultipleObjectsEx(static_cast<DWORD>(wakeupHandles.size()), wakeupHandles.begin(), timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + wakeupHandles.size()))
            {
                commandDispatcher.Dispatch(*m_CommandQueue);
            }
            else if (result == WAIT_OBJECT_0 + wakeupHandles.size())
            {
                if (!OSMessageDispatch())
                {
                    // [重要] XAudio2がリソースを参照中なのにリソースを解放してはいけない
                    m_AudioProcessor->WaitForAllStop();
                    return;
                }
            }

            isAnyActive = m_AudioProcessor->Process();
            m_DiagnosticsReporter.Submit(m_DiagnosticsSink);

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

    auto Engine::WaitForThreadStop() -> void
    {
        if (m_AudioThread.joinable())
        {
            m_AudioThread.join();
        }
    }

} // namespace N503::Audio
