#include "Pch.hpp"
#include "ResidualBuffer.hpp"

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <cstddef>
#include <span>
#include <string.h>

namespace N503::Audio::Codec
{

    auto ResidualBuffer::Read(std::span<std::byte> target) -> std::size_t
    {
        auto available = std::span(m_Cache).subspan(m_ReadOffset);

        if (available.empty() || target.empty())
        {
            return 0;
        }

        const std::size_t bytesToRead = std::min(available.size(), target.size());

        std::ranges::copy_n(available.begin(), bytesToRead, target.begin());
        m_ReadOffset += bytesToRead;

        if (m_ReadOffset >= m_Cache.size())
        {
            Clear();
        }

        return bytesToRead;
    }

    void ResidualBuffer::Write(std::span<const std::byte> data)
    {
        if (!data.empty())
        {
            m_Cache.insert(m_Cache.end(), data.begin(), data.end());
        }
    }

    void ResidualBuffer::Clear()
    {
        m_Cache.clear();
        m_ReadOffset = 0;
    }

} // namespace N503::Audio::Codec
