#include "Pch.hpp"
#include "Pause.hpp"

// 1. Project Headers
#include "../../Engine.hpp"
#include "../../System/Processor.hpp"
#include "../Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Message::Packets
{

    auto Pause::operator()(const Context& context) const -> void
    {
        context.SystemProcessor.Pause(Handle);
    }

} // namespace N503::Audio::Message::Packets
