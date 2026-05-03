#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstddef>
#include <span>
#include <vector>

namespace N503::Audio::Codec
{

    class ResidualBuffer
    {
    public:
        auto Read(std::span<std::byte> target) -> std::size_t;

        auto Write(std::span<const std::byte> data) -> void;

        auto Clear() -> void;

        auto GetAvailableBytes() const -> std::size_t
        {
            return m_Cache.size();
        }

    private:
        std::vector<std::byte> m_Cache;

        std::size_t m_ReadOffset{ 0 };
    };

} // namespace N503::Audio::Codec
