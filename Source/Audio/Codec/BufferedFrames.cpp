#include "Pch.hpp"
#include "BufferedFrames.hpp"

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstddef>
#include <span>
#include <stdexcept>
#include <string.h>

namespace N503::Audio::Codec
{

    BufferedFrames::BufferedFrames(std::byte *baseAddress, std::size_t totalSize)
        : m_BaseAddress(baseAddress), m_BytesSize(totalSize)
    {
        if (!baseAddress || totalSize == 0)
        {
            throw std::invalid_argument("BufferedFrames initialized with invalid memory.");
        }
    }

    auto BufferedFrames::GetView(std::size_t offset, std::size_t length) -> std::span<std::byte>
    {
        if (offset + length > m_BytesSize)
        {
            throw std::out_of_range("View range exceeds total buffer size.");
        }

        return std::span<std::byte>(m_BaseAddress + offset, length);
    }

    auto BufferedFrames::GetBaseAddress() const -> std::byte *
    {
        return m_BaseAddress;
    }

    auto BufferedFrames::GetBytesSize() const -> std::size_t
    {
        return m_BytesSize;
    }

    void BufferedFrames::Write(std::span<const std::byte> data, std::size_t offset)
    {
        if (offset + data.size() > m_BytesSize)
        {
            throw std::out_of_range("Write operation exceeds buffer capacity.");
        }

        std::memcpy(m_BaseAddress + offset, data.data(), data.size());
    }

} // namespace N503::Audio::Codec
