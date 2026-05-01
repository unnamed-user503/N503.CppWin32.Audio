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
#include <span>

namespace N503::Audio::Frames
{

    struct Buffer final
    {
        std::span<std::byte> Bytes{};

        std::uint64_t Count{ 0 };

        bool IsEndOfStream{ false };
    };

} // namespace N503::Audio::Frames
