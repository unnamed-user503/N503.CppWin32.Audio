#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

// Forward Declarations
namespace N503::Audio::Message
{
    struct Context;
}

namespace N503::Audio::Message::Packets
{

    struct Pause final
    {
        Audio::ProcessHandle Handle;

        auto operator()(const Context& context) const -> void;
    };

} // namespace N503::Audio::Command::Packets
