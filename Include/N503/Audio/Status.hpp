#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>

namespace N503::Audio
{

    enum class Status : std::uint16_t
    {
        Stopped = 0,
        Loading,
        Playing,
        Paused,
        Stopping,
        Error
    };

} // namespace N503::Audio
