#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>
#include <cstddef>
#include <cstdint>

namespace N503::Audio::Frames
{

    struct Buffer final
    {
        std::byte* Bytes{ nullptr };

        std::size_t Size{ 0 };

        std::uint64_t Count{ 0 };

        std::chrono::duration<double> Duration{ 0.0 };

        bool IsEndOfStream{ false };
    };

} // namespace N503::Audio::Frames
