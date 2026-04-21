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

    struct Attributes
    {
        float Volume{1.0f};

        float Pitch{1.0f};

        std::int32_t Repeat{0};
    };

} // namespace N503::Audio
