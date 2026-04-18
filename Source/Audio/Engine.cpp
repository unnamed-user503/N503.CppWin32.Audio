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
            signal.release();
            this->Run(std::move(stopToken));
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
        // reporter.AddSink(std::make_shared<Diagnostics::FileSink>("log.txt"));

        auto CoUninitializeReservedCall = wil::CoInitializeEx();
        ::MFStartup(MF_VERSION, MFSTARTUP_LITE);

        m_DeviceContext = std::make_unique<Device::Context>();
        m_AudioProcessor = std::make_unique<Audio::Processor>();

        Command::Dispatcher commandDispatcher;
        Command::Executor commandExecutor;

        auto wakeupHandles = { m_CommandQueue->GetWakeupEventHandle() };

        while (!stopToken.stop_requested())
        {
            if (!m_CommandQueue->IsEmpty())
            {
                try
                {
                    commandDispatcher.Dispatch(*m_CommandQueue, commandExecutor);
                }
                CATCH_LOG();
            }

            auto isAnyActive = m_AudioProcessor->Process();
            reporter.Submit(m_DiagnosticsSink);

            if (m_CommandQueue->IsEmpty() && !isAnyActive)
            {
                auto result = ::WaitForMultipleObjectsEx(static_cast<DWORD>(wakeupHandles.size()), wakeupHandles.begin(), FALSE, INFINITE, FALSE);

                if (result >= WAIT_OBJECT_0 && result < (WAIT_OBJECT_0 + wakeupHandles.size()))
                {
                    continue;
                }
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(1));
            }
        }

        ::MFShutdown();

        // スレッドが終了するのでフラグを下げて置く
        m_IsThreadRunning.store(true, std::memory_order_release);
    }

} // namespace N503::Audio
