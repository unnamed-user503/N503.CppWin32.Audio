#include "Pch.hpp"
#include "Resume.hpp"

// 1. Project Headers
#include "../../System/Processor.hpp"
#include "../Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Message::Packets
{

    auto Resume::operator()(const Context& context) const -> void
    {
        context.SystemProcessor.Resume(Handle);
    }

} // namespace N503::Audio::Message::Packets
