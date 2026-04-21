#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstddef>
#include <span>

namespace N503::Audio::Codec
{

    class BufferedFrames
    {
    public:
        BufferedFrames(std::byte *baseAddress, std::size_t bytesSize);

        auto GetView(std::size_t offset, std::size_t length) -> std::span<std::byte>;

        auto GetBaseAddress() const -> std::byte *;

        auto GetBytesSize() const -> std::size_t;

        void Write(std::span<const std::byte> data, std::size_t offset = 0);

    private:
        std::byte *m_BaseAddress;

        std::size_t m_BytesSize;
    };

} // namespace N503::Audio::Codec
