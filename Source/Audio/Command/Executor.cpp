#include "Pch.hpp"
#include "Executor.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "../Processor.hpp"
#include "../Resource/Container.hpp"
#include "Packets/CreateResourceCommand.hpp"
#include "Packets/DestroyResourceCommand.hpp"
#include "Packets/PauseCommand.hpp"
#include "Packets/PlayCommand.hpp"
#include "Packets/ResumeCommand.hpp"
#include "Packets/StopCommand.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>
#include <N503/Diagnostics/Severity.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Audio::Command
{

    auto Executor::operator()(const Packets::CreateResourceCommand& command) -> void
    {
        if (!command.Result)
        {
#ifdef _DEBUG
            Audio::Engine::Instance().GetDiagnosticsSink().AddEntry({ Diagnostics::Severity::Error, "[Audio] Executor: command.Result is empty." });
#endif
            return;
        }

        auto handle = Engine::Instance().GetResourceContainer().Store(command.Type, std::string(command.Path));

        if (!handle)
        {
            command.Result->ResourceID = Audio::Handle::ResourceID::InvalidValue;
            return;
        }
        else
        {
            *command.Result = handle;
        }
    }

    auto Executor::operator()(const Packets::DestroyResourceCommand& command) -> void
    {
        Audio::Engine::Instance().GetAudioProcessor().Stop(command.ProcessHandle);
    }

    auto Executor::operator()(const Packets::PlayCommand& command) -> void
    {
        if (auto asset = Audio::Engine::Instance().GetResourceContainer().GetAsset(command.Handle))
        {
            *command.Result = Audio::Engine::Instance().GetAudioProcessor().Play(asset);
        }
    }

    auto Executor::operator()(const Packets::StopCommand& command) -> void
    {
        Audio::Engine::Instance().GetAudioProcessor().Stop(command.Handle);
    }

    auto Executor::operator()(const Packets::PauseCommand& command) -> void
    {
        Audio::Engine::Instance().GetAudioProcessor().Pause(command.Handle);
    }

    auto Executor::operator()(const Packets::ResumeCommand& command) -> void
    {
        Audio::Engine::Instance().GetAudioProcessor().Resume(command.Handle);
    }

}; // namespace N503::Audio::Command
