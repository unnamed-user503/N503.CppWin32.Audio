#pragma once

// 1. Project Headers
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <Windows.h>
#include <atomic>
#include <memory>
#include <stop_token>
#include <thread>

namespace N503::Audio::Message
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

    class Engine final
    {
    public:
        static auto GetInstance() -> Engine&;

    public:
        ~Engine();

        auto Start() -> void;

        auto Stop() -> void;

        auto Wait() -> void;

        auto GetMessageQueue() const noexcept -> Message::Queue&
        {
            return *m_MessageQueue;
        }

        auto GetResourceContainer() const noexcept -> Resource::Container&
        {
            return *m_ResourceContainer;
        }

        auto GetDeviceContext() const noexcept -> Device::Context&
        {
            return *m_DeviceContext;
        }

        auto GetAudioProcessor() noexcept -> Audio::Processor&
        {
            return *m_AudioProcessor;
        }

        auto GetDiagnosticsSink() noexcept -> Diagnostics::Sink&
        {
            return m_DiagnosticsSink;
        }

    private:
        Engine();

        auto Run(const std::stop_token stopToken) -> void;

    private:
        std::atomic<bool> m_IsRunning{ false };

        wil::unique_event m_StartedEvent{ ::CreateEventW(nullptr, TRUE, FALSE, L"Local\\N503.CppWin32.Audio.Event.EngineStarted") };

        std::unique_ptr<Message::Queue> m_MessageQueue;

        std::unique_ptr<Resource::Container> m_ResourceContainer;

        std::unique_ptr<Device::Context> m_DeviceContext;

        std::unique_ptr<Audio::Processor> m_AudioProcessor;

        Diagnostics::Sink m_DiagnosticsSink;

        std::jthread m_AudioThread;
    };

} // namespace N503::Audio
