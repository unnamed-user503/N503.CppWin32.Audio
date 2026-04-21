#include "Pch.hpp"
#include "BufferAllocator.hpp"

// 1. Project Headers
#include "BufferedFrames.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstddef>
#include <exception>

namespace N503::Audio::Codec
{

    BufferAllocator::BufferAllocator(std::size_t capacity) : m_Storage(capacity)
    {
    }

    auto BufferAllocator::Allocate(std::size_t blockSize) -> BufferedFrames
    {
        auto allocatedBuffer = m_Storage.AllocateBytes(blockSize, 16);

        if (!allocatedBuffer)
        {
            throw std::bad_alloc();
        }

        return {static_cast<std::byte *>(allocatedBuffer), blockSize};
    }

    auto BufferAllocator::Deallocate(const BufferedFrames) -> void
    {
        /* nop */
    }

} // namespace N503::Audio::Codec
