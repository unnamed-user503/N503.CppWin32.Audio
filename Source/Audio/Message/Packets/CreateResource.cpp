#include "Pch.hpp"
#include "CreateResource.hpp"

// 1. Project Headers
#include "../../Engine.hpp"
#include "../Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>
#include <N503/Diagnostics/Severity.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Audio::Message::Packets
{

    auto CreateResource::operator()(const Context& context) const -> void
    {
        if (!Result)
        {
#ifdef _DEBUG
            const Diagnostics::Entry entry = { Diagnostics::Severity::Error, "[Audio] <CreateResource> : command.Result is empty." };
            Audio::Engine::GetInstance().GetDiagnosticsSink().AddEntry(entry);
#endif
            return;
        }

        auto handle = Audio::Engine::GetInstance().GetResourceContainer().Store(Type, std::string(Path));

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
