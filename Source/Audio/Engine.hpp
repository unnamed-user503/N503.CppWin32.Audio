#pragma once

// 1. Project Headers
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <atomic>
#include <memory>
#include <stop_token>
#include <thread>

namespace N503::Audio::Command
{
    class Queue;
}

namespace N503::Audio::Device
{
    class Context;
}

namespace N503::Audio
{
    class Processor;
}

namespace N503::Audio
{

    /// @brief
    class Engine final
    {
    public:
        /// @brief
        /// @return
        static auto Instance() -> Engine&;

    public:
        /// @brief
        ~Engine();

        /// @brief
        /// @return
        auto Start() -> void;

        /// @brief
        /// @return
        auto Stop() -> void;

        /// @brief 
        /// @return 
        auto WaitForThreadStop() -> void;

        /// @brief
        /// @return
        auto GetCommandQueue() const noexcept -> Command::Queue&
        {
            return *m_CommandQueue;
        }

        /// @brief
        /// @return
        auto GetResourceContainer() const noexcept -> Resource::Container&
        {
            return *m_ResourceContainer;
        }

        /// @brief
        /// @return
        auto GetDeviceContext() const noexcept -> Device::Context&
        {
            return *m_DeviceContext;
        }

        /// @brief
        /// @return
        auto GetAudioProcessor() noexcept -> Audio::Processor&
        {
            return *m_AudioProcessor;
        }

        /// @brief
        /// @return
        auto GetDiagnosticsSink() noexcept -> Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

        /// @brief
        /// @return
        auto GetDiagnosticsReporter() noexcept -> Diagnostics::Reporter&
        {
            return m_DiagnosticsReporter;
        }

    private:
        /// @brief
        Engine();

        /// @brief
        /// @return
        auto Run(const std::stop_token stopToken) -> void;

    private:
        /// @brief
        std::atomic<bool> m_IsThreadRunning{ false };

        /// @brief
        std::unique_ptr<Command::Queue> m_CommandQueue;

        /// @brief
        std::unique_ptr<Resource::Container> m_ResourceContainer;

        /// @brief
        std::unique_ptr<Device::Context> m_DeviceContext;

        /// @brief
        std::unique_ptr<Audio::Processor> m_AudioProcessor;

        /// @brief
        Diagnostics::Sink m_DiagnosticsSink;

        /// @brief 
        Diagnostics::Reporter m_DiagnosticsReporter;

        /// @brief
        std::jthread m_AudioThread;
    };

} // namespace N503::Audio
