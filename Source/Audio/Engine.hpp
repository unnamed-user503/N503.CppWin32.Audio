#pragma once

#include "Resource/Container.hpp"

#include <N503/Diagnostics/Sink.hpp>

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
        auto GetDiagnostics() noexcept -> Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

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
        std::jthread m_AudioThread;

        /// @brief
        Diagnostics::Sink m_DiagnosticsSink;

        /// @brief
        std::unique_ptr<Command::Queue> m_CommandQueue;

        /// @brief
        std::unique_ptr<Resource::Container> m_ResourceContainer;

        /// @brief
        std::unique_ptr<Device::Context> m_DeviceContext;

        /// @brief
        std::unique_ptr<Audio::Processor> m_AudioProcessor;
    };

} // namespace N503::Audio
