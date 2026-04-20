#include "Pch.hpp"
#include "Engine.hpp"

// 依存関係のインクルード(内部プロジェクト)
#include "Command/Dispatcher.hpp"
#include "Command/Executor.hpp"
#include "Command/Queue.hpp"
#include "Device/Context.hpp"
#include "Processor.hpp"
#include "Resource/Container.hpp"

// 依存関係のインクルード(外部プロジェクト)
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Reporter.hpp>

// C++ 標準ライブラリのインクルード
#include <chrono>
#include <memory>
#include <semaphore>
#include <stop_token>
#include <thread>
#include <utility>

// Windows ヘッダーのインクルード
#include <Windows.h>
#include <mfapi.h>

// WIL (Windows Implementation Library)
#include <wil/resource.h>
#include <wil/result_macros.h>
#include <iostream>
#include <format>

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
            // スレッドが開始されたのでロックを解放する
            signal.release();

            // スレッドのメッセージキューを強制する
            MSG msg;
            ::PeekMessage(&msg, NULL, WM_USER, WM_USER, PM_NOREMOVE);

            // COMの初期化
            auto coinit = wil::CoInitializeEx(COINIT_MULTITHREADED);
            ::MFStartup(MF_VERSION, MFSTARTUP_LITE);
            auto mfshutdown = wil::scope_exit([]{ ::MFShutdown(); });

            // オーディオスレッド終了用のシグナル
            wil::unique_event done{ ::CreateEventW(nullptr, FALSE, FALSE, L"Local\\N503.CppWin32.Event.Audio.Done") };
            // オーディオスレッドの開始
            this->Run(std::move(stopToken));
            // オーディオスレッドが終了した事を示す
            done.SetEvent();
            ::OutputDebugStringA("************* AudioThread END OF THREAD FUNCTION ***********************************\n");
        });

        // スレッドが開始されるのを待つ
        signal.acquire();

        // スレッドを開始したのでフラグを立てておく
        m_IsThreadRunning.store(true, std::memory_order_release);
    }

    auto Engine::Stop() -> void
    {
    }

    auto Engine::Run(const std::stop_token stopToken) -> void
    {
        Diagnostics::Reporter reporter{};
        reporter.AddSink(std::make_shared<Diagnostics::ConsoleSink>());

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
        Command::Executor commandExecutor;

        bool isAnyActive = false;

        while (!stopToken.stop_requested())
        {
            auto timeout = isAnyActive ? 0 : INFINITE;
            auto result = ::MsgWaitForMultipleObjectsEx(static_cast<DWORD>(wakeupHandles.size()), wakeupHandles.begin(), timeout, QS_ALLINPUT, MWMO_INPUTAVAILABLE);

            if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + wakeupHandles.size()))
            {
                commandDispatcher.Dispatch(*m_CommandQueue, commandExecutor);
            }
            else if (result == WAIT_OBJECT_0 + wakeupHandles.size())
            {
                if (!OSMessageDispatch())
                {
                    m_AudioProcessor->StopAll();
                    return;
                }
            }

            isAnyActive = m_AudioProcessor->Process();
            reporter.Submit(m_DiagnosticsSink);

            //std::this_thread::yield();
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

} // namespace N503::Audio
