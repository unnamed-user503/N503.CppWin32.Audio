#include "Pch.hpp"
#include "CreateResourceCommand.hpp"

// 1. Project Headers
#include "../../Engine.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>
#include <N503/Diagnostics/Severity.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Audio::Command::Packets
{

    auto CreateResourceCommand::operator()() const -> void
    {
        if (!Result)
        {
#ifdef _DEBUG
            Audio::Engine::Instance().GetDiagnosticsSink().AddEntry({ Diagnostics::Severity::Error, "[Audio] CreateResourceCommand: command.Result is empty." }
            );
#endif
            return;
        }

        auto handle = Audio::Engine::Instance().GetResourceContainer().Store(Type, std::string(Path));

        if (!handle)
        {
            Result->ResourceID = Audio::Handle::ResourceID::InvalidValue;
            return;
        }
        else
        {
            *Result = handle;
        }
    }

} // namespace N503::Audio::Command::Packets
