#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <atomic>

namespace N503::Audio::Device
{

    struct Signal
    {
        enum Event : int
        {
            None = 0,
            BufferStart,
            BufferEnd,
        };

        std::atomic<bool> Notify{ false };

        std::atomic<Event> Event{ Event::None };
    };

} // namespace N503::Audio::Device
