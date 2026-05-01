#include "Pch.hpp"
#include "DestroyResource.hpp"

// 1. Project Headers
#include "../../Engine.hpp"
#include "../../Processor.hpp"
#include "../Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Message::Packets
{

    auto DestroyResource::operator()(const Context& context) const -> void
    {
        Audio::Engine::GetInstance().GetAudioProcessor().Stop(ProcessHandle);
    }

} // namespace N503::Audio::Message::Packets
