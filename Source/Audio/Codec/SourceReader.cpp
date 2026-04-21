#include "Pch.hpp"
#include "SourceReader.hpp"

// 1. Project Headers
#include "Metadata.hpp"
#include "ResidualBuffer.hpp"
#include "StreamSelector.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/resource.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <mfobjects.h>
#include <mfreadwrite.h>

// 6. C++ Standard Libraries
#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <format>
#include <memory>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>

namespace N503::Audio::Codec
{

    namespace
    {
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    } // namespace

    SourceReader::SourceReader(std::string_view path)
    {
        auto wide = TranscodeUtf8ToWide(path);

        auto attributes = ::GetFileAttributesW(wide.data());
        if (attributes == INVALID_FILE_ATTRIBUTES)
        {
            auto error = ::GetLastError();
            throw std::runtime_error(
                std::format("[Audio] StreamReader: Failed to access file (Error: {}): {}", error, path)
            );
        }

        THROW_IF_FAILED(::MFCreateSourceReaderFromURL(wide.data(), nullptr, m_ConcreteReader.put()));

        m_StreamSelector = std::make_unique<StreamSelector>(m_ConcreteReader);
        m_StreamSelector->SelectFirst();

        m_Metadata = std::make_unique<Metadata>(m_ConcreteReader);
        m_ResidualBuffer = std::make_unique<ResidualBuffer>();
    }

    auto SourceReader::ReadBytes(std::span<std::byte> target) -> std::size_t
    {
        std::size_t totalBytesFilled = 0;
        const std::size_t totalBytesNeeded = target.size();

        totalBytesFilled += m_ResidualBuffer->Read(target);

        while (totalBytesFilled < totalBytesNeeded)
        {
            wil::com_ptr<IMFSample> mfSample;
            DWORD streamFlags = 0;

            THROW_IF_FAILED(m_ConcreteReader->ReadSample(
                MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, nullptr, &streamFlags, nullptr, mfSample.put()
            ));

            if (streamFlags & MF_SOURCE_READERF_ENDOFSTREAM)
            {
                break;
            }
            if (!mfSample)
            {
                continue;
            }

            wil::com_ptr<IMFMediaBuffer> mfMediaBuffer;
            THROW_IF_FAILED(mfSample->ConvertToContiguousBuffer(mfMediaBuffer.put()));

            BYTE *pAudioData = nullptr;
            DWORD audioBytes = 0;
            THROW_IF_FAILED(mfMediaBuffer->Lock(&pAudioData, nullptr, &audioBytes));

            auto unlock = wil::scope_exit([&] { mfMediaBuffer->Unlock(); });

            const std::size_t bytesAvailable = static_cast<std::size_t>(audioBytes);
            const std::size_t spaceRemaining = totalBytesNeeded - totalBytesFilled;
            const std::size_t bytesToCopy = std::min(bytesAvailable, spaceRemaining);

            auto sourceView = std::span{reinterpret_cast<const std::byte *>(pAudioData), bytesAvailable};
            auto targetView = target.subspan(totalBytesFilled);

            std::ranges::copy_n(sourceView.begin(), bytesToCopy, targetView.begin());
            totalBytesFilled += bytesToCopy;

            if (bytesToCopy < bytesAvailable)
            {
                m_ResidualBuffer->Write(sourceView.subspan(bytesToCopy));
            }
        }

        return totalBytesFilled;
    }

    auto SourceReader::Seek(std::uint32_t frameIndex) -> void
    {
        // 事前条件のチェック（そもそもリーダーがないならロジックエラー）
        if (!m_ConcreteReader)
        {
            throw std::runtime_error("SourceReader::Seek: m_ConcreteReader is null.");
        }

        m_ResidualBuffer->Clear();

        // 単位変換：フレーム -> 100ns 単位
        PROPVARIANT variant{};
        variant.vt = VT_I8;
        variant.hVal.QuadPart = (static_cast<LONGLONG>(frameIndex) * 10000000LL) /
                                m_Metadata->GetFormat().SamplePerSecond;

        // Media Foundation のシークは失敗したら「異常事態」として扱う
        THROW_IF_FAILED(m_ConcreteReader->SetCurrentPosition(GUID_NULL, variant));
    }

} // namespace N503::Audio::Codec
