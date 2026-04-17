#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <mfreadwrite.h>

// 6. C++ Standard Libraries
#include <cstdint>

namespace N503::Audio::Codec
{

    class Metadata
    {
    public:
        explicit Metadata(const wil::com_ptr<IMFSourceReader>& reader);

        auto Update() -> void;

        auto GetFormat() const -> const Audio::Format&;

        auto GetTotalSamples() const -> std::uint32_t;

        auto GetTotalFrames() const -> std::uint32_t;

        auto GetTotalDurations() const -> double;

    private:
        wil::com_ptr<IMFSourceReader> m_SourceReader;

        Audio::Format m_Format{};

        std::uint32_t m_TotalSamples{};

        std::uint32_t m_TotalFrames{};

        double m_DurationSeconds{};
    };

} // namespace N503::Audio::Codec
