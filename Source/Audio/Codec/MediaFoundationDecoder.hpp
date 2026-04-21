#pragma once

// 1. Project Headers
#include "../Frames/Buffer.hpp"
#include "BufferAllocator.hpp"
#include "Decoder.hpp"
#include "SourceReader.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <memory>
#include <string_view>

namespace N503::Audio::Codec
{

    class MediaFoundationDecoder final : public Decoder
    {
    public:
        explicit MediaFoundationDecoder(std::string_view url);

        virtual ~MediaFoundationDecoder() override = default;

        auto Decode(std::uint32_t framesRequested) -> Frames::Buffer override;

        auto Decode(std::uint32_t framesRequested, Allocator allocator) -> Frames::Buffer override;

        auto Seek(std::uint32_t frames) -> void override;

        auto GetFormat() const -> const Audio::Format & override;

        auto GetTotalSamples() const -> std::uint32_t override;

        auto GetTotalFrames() const -> std::uint32_t override;

        auto GetTotalDurations() const -> double override;

    private:
        std::unique_ptr<SourceReader> m_SourceReader;

        std::unique_ptr<BufferAllocator> m_BufferAllocator;
    };

} // namespace N503::Audio::Codec
