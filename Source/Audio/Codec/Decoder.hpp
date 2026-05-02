#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <Audio/Frames/Buffer.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstddef>
#include <cstdint>
#include <functional>
#include <span>

namespace N503::Audio::Codec
{

    class Decoder
    {
    public:
        using Allocator = std::function<std::span<std::byte>(std::size_t)>;

    public:
        virtual ~Decoder() = default;

        virtual auto Decode(std::uint32_t framesRequested) -> Frames::Buffer = 0;

        virtual auto Decode(std::uint32_t framesRequested, Allocator allocator) -> Frames::Buffer = 0;

        virtual auto Seek(std::uint32_t frames) -> void = 0;

        virtual auto GetFormat() const -> const Audio::Format& = 0;

        virtual auto GetTotalSamples() const -> std::uint32_t = 0;

        virtual auto GetTotalFrames() const -> std::uint32_t = 0;

        virtual auto GetTotalDurations() const -> double = 0;
    };

} // namespace N503::Audio::Codec
