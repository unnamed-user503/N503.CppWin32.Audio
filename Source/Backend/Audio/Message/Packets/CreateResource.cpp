#include "Pch.hpp"
#include "CreateResource.hpp"

// 1. Project Headers
#include "../../Engine.hpp"
#include "../Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>
#include <N503/Diagnostics/Reporter.hpp>

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
            Audio::Engine::GetInstance().GetDiagnosticsReporter().Error(L"[Audio]<CreateResource::operator()>: Result is empty.");
#endif
            return;
        }

        auto handle = context.ResourceContainer.Store(Type, std::string(Path));

        if (!handle)
        {
            Result->ResourceID = Identity::ResourceID::Invalid;
            return;
        }
        else
        {
            *Result = handle;
        }
    }

} // namespace N503::Audio::Message::Packets
