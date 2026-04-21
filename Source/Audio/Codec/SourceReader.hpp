#pragma once

// 1. Project Headers
#include "Metadata.hpp"
#include "ResidualBuffer.hpp"
#include "StreamSelector.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <mfreadwrite.h>

// 6. C++ Standard Libraries
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

namespace N503::Audio::Codec
{

    class SourceReader
    {
    public:
        explicit SourceReader(std::string_view url);

        auto ReadBytes(std::span<std::byte> target) -> std::size_t;

        auto Seek(std::uint32_t frameIndex) -> void;

        auto GetMetadata() const -> const Metadata &
        {
            return *m_Metadata;
        }

    private:
        wil::com_ptr<IMFSourceReader> m_ConcreteReader;

        std::unique_ptr<StreamSelector> m_StreamSelector;

        std::unique_ptr<Metadata> m_Metadata;

        std::unique_ptr<ResidualBuffer> m_ResidualBuffer;
    };

} // namespace N503::Audio::Codec
