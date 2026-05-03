#include "Pch.hpp"
#include "MediaFoundationDecoder.hpp"

// 1. Project Headers
#include "../Frames/Buffer.hpp"
#include "BufferAllocator.hpp"
#include "Decoder.hpp"
#include "SourceReader.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <string_view>

namespace N503::Audio::Codec
{

    MediaFoundationDecoder::MediaFoundationDecoder(std::string_view url)
        : m_SourceReader(std::make_unique<SourceReader>(url)), m_BufferAllocator(std::make_unique<BufferAllocator>(1024))
    {
    }

    auto MediaFoundationDecoder::Decode(std::uint32_t framesRequested) -> Frames::Buffer
    {
        const auto& format = GetFormat();

        const std::size_t bytesPerFrame    = format.Channels * (format.BitsPerSample / 8);
        const std::size_t totalBytesNeeded = framesRequested * bytesPerFrame;

        auto bufferedFrames = m_BufferAllocator->Allocate(totalBytesNeeded);

        std::size_t bytesRead = m_SourceReader->ReadBytes(bufferedFrames.GetView(0, totalBytesNeeded));

        Frames::Buffer result;
        result.Count = static_cast<std::uint32_t>(bytesRead / bytesPerFrame);
        result.Bytes = { reinterpret_cast<std::byte*>(bufferedFrames.GetBaseAddress()), bytesRead };

        return result;
    }

    auto MediaFoundationDecoder::Decode(std::uint32_t framesRequested, Allocator allocator) -> Frames::Buffer
    {
        const auto& format = GetFormat();

        const std::size_t bytesPerFrame    = format.Channels * (format.BitsPerSample / 8);
        const std::size_t totalBytesNeeded = framesRequested * bytesPerFrame;

        auto target = allocator(totalBytesNeeded);

        if (target.empty() || target.size() < totalBytesNeeded)
        {
            return {};
        }

        std::size_t writtenBytes = m_SourceReader->ReadBytes(target);

        std::uint32_t decodedFrames = static_cast<std::uint32_t>(writtenBytes / format.BlockAlign);

        return { .Bytes = { target.data(), writtenBytes }, .Count = decodedFrames, .IsEndOfStream = (decodedFrames == 0) };
    }

    void MediaFoundationDecoder::Seek(std::uint32_t frames)
    {
        m_SourceReader->Seek(frames);
    }

    auto MediaFoundationDecoder::GetFormat() const -> const Audio::Format&
    {
        return m_SourceReader->GetMetadata().GetFormat();
    }

    auto MediaFoundationDecoder::GetTotalSamples() const -> std::uint32_t
    {
        return m_SourceReader->GetMetadata().GetTotalSamples();
    }

    auto MediaFoundationDecoder::GetTotalFrames() const -> std::uint32_t
    {
        return m_SourceReader->GetMetadata().GetTotalFrames();
    }

    auto MediaFoundationDecoder::GetTotalDurations() const -> double
    {
        return m_SourceReader->GetMetadata().GetTotalDurations();
    }

} // namespace N503::Audio::Codec
