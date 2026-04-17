#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Command::Packets
{

    struct StopCommand final
    {
        Audio::ProcessHandle Handle;
    };

} // namespace N503::Audio::Command::Packets
