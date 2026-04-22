#pragma once

// 1. Project Headers
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Command
{

    class Dispatcher final
    {
    public:
        auto Dispatch(Command::Queue& queue) -> void;
    };

} // namespace N503::Audio::Command
