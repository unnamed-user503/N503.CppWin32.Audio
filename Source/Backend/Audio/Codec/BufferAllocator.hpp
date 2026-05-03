#pragma once

// 1. Project Headers
#include "BufferedFrames.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Arena.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Codec
{

    class BufferedFrame;

}; // namespace N503::Audio::Codec

namespace N503::Audio::Codec
{

    class BufferAllocator
    {
    public:
        explicit BufferAllocator(std::size_t capacity);

        auto Allocate(std::size_t blockSize) -> BufferedFrames;

        auto Deallocate(const BufferedFrames) -> void;

    private:
        N503::Memory::Storage::Arena m_Storage;
    };

} // namespace N503::Audio::Codec
