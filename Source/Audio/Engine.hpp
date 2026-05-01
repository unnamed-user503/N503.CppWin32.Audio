#pragma once

// 1. Project Headers
#include "Resource/Container.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>

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
    class MasterVoice;
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

        auto Start() -> bool;

        auto Stop() -> bool;

        auto Wait() -> bool;

        auto GetMessageQueue() const noexcept -> Message::Queue&
        {
            return *m_MessageQueue;
        }

        auto GetResourceContainer() const noexcept -> Resource::Container&
        {
            return *m_ResourceContainer;
        }

        auto GetMasterVoice() const noexcept -> Device::MasterVoice&
        {
            return *m_MasterVoice;
        }

        auto GetAudioProcessor() noexcept -> Audio::Processor&
        {
            return *m_AudioProcessor;
        }

        auto GetDiagnosticsReporter() noexcept -> Diagnostics::Reporter&
        {
            return *m_DiagnosticsReporter;
        }

    private:
        Engine();

        auto Run(const std::stop_token stopToken) -> void;

    private:
        std::atomic<bool> m_IsRunning{ false };

        wil::unique_event m_StartedEvent{ ::CreateEventW(nullptr, TRUE, FALSE, L"Local\\N503.CppWin32.Audio.Event.EngineStarted") };

        std::unique_ptr<Message::Queue> m_MessageQueue;

        std::unique_ptr<Resource::Container> m_ResourceContainer;

        std::unique_ptr<Device::MasterVoice> m_MasterVoice;

        std::unique_ptr<Audio::Processor> m_AudioProcessor;

        std::unique_ptr<Diagnostics::Reporter> m_DiagnosticsReporter;

        std::jthread m_AudioThread;
    };

} // namespace N503::Audio
