#include "Pch.hpp"
#include "Metadata.hpp"

// 1. Project Headers
#include <N503/Audio/Format.hpp>

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <mfapi.h>
#include <mfidl.h>
#include <mfobjects.h>
#include <mfreadwrite.h>
#include <mmreg.h>
#include <strsafe.h>

// 6. C++ Standard Libraries
#include <cmath>
#include <cstdint>

namespace N503::Audio::Codec
{

    Metadata::Metadata(const wil::com_ptr<IMFSourceReader> &reader) : m_SourceReader(reader)
    {
        Update();
    }

    auto Metadata::Update() -> void
    {
        wil::com_ptr<IMFMediaType> mfMediaType;
        THROW_IF_FAILED(m_SourceReader->GetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, mfMediaType.put()));

        GUID subtype = {0};
        UINT32 channels = 0;
        UINT32 sampleRate = 0;
        UINT32 bitsPerSample = 0;
        UINT32 avgBytesPerSec = 0;
        UINT32 blockAlign = 0;

        THROW_IF_FAILED(mfMediaType->GetUINT32(MF_MT_AUDIO_NUM_CHANNELS, &channels));
        THROW_IF_FAILED(mfMediaType->GetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, &sampleRate));
        THROW_IF_FAILED(mfMediaType->GetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, &bitsPerSample));
        THROW_IF_FAILED(mfMediaType->GetUINT32(MF_MT_AUDIO_AVG_BYTES_PER_SECOND, &avgBytesPerSec));
        THROW_IF_FAILED(mfMediaType->GetUINT32(MF_MT_AUDIO_BLOCK_ALIGNMENT, &blockAlign));

        PROPVARIANT var;
        PropVariantInit(&var);
        if (SUCCEEDED(m_SourceReader->GetPresentationAttribute(MF_SOURCE_READER_MEDIASOURCE, MF_PD_DURATION, &var)))
        {
            m_DurationSeconds = static_cast<double>(var.uhVal.QuadPart) / 10000000.0;
            PropVariantClear(&var);
        }

        if (channels == 0 || sampleRate == 0 || bitsPerSample == 0)
        {
            WAVEFORMATEX *pWaveFormat{};
            std::uint32_t waveFormatSize{};
            THROW_IF_FAILED(::MFCreateWaveFormatExFromMFMediaType(
                mfMediaType.get(), &pWaveFormat, &waveFormatSize, MFWaveFormatExConvertFlag_Normal
            ));

            m_Format.Tag = pWaveFormat->wFormatTag;
            m_Format.BlockAlign = pWaveFormat->nBlockAlign;
            m_Format.Channels = pWaveFormat->nChannels;
            m_Format.SamplePerSecond = pWaveFormat->nSamplesPerSec;
            m_Format.BitsPerSample = pWaveFormat->wBitsPerSample;
            m_Format.BytesPerSecond = pWaveFormat->nAvgBytesPerSec;

            ::CoTaskMemFree(pWaveFormat);
            return;
        }

        if (blockAlign == 0)
        {
            blockAlign = static_cast<UINT32>(channels * (bitsPerSample / 8));
        }

        THROW_IF_FAILED(mfMediaType->GetGUID(MF_MT_SUBTYPE, &subtype));

        m_Format.Tag = WAVE_FORMAT_UNKNOWN;
        m_Format.Channels = static_cast<std::uint16_t>(channels);
        m_Format.SamplePerSecond = sampleRate;
        m_Format.BitsPerSample = static_cast<std::uint16_t>(bitsPerSample);
        m_Format.BlockAlign = static_cast<std::uint16_t>(blockAlign);
        m_Format.BytesPerSecond = avgBytesPerSec;

        if (subtype == MFAudioFormat_Float)
        {
            m_Format.Tag = WAVE_FORMAT_IEEE_FLOAT;
        }
        else if (subtype == MFAudioFormat_PCM)
        {
            m_Format.Tag = WAVE_FORMAT_PCM;
        }
        else if (subtype == MFAudioFormat_MP3)
        {
            m_Format.Tag = WAVE_FORMAT_MPEGLAYER3;
        }
        else
        {
            m_Format.Tag = WAVE_FORMAT_EXTENSIBLE;
        }
    }

    auto Metadata::GetFormat() const -> const Audio::Format &
    {
        return m_Format;
    }

    auto Metadata::GetTotalSamples() const -> std::uint32_t
    {
        return GetTotalFrames() * m_Format.Channels;
    }

    auto Metadata::GetTotalFrames() const -> std::uint32_t
    {
        return static_cast<std::uint32_t>(std::round(m_DurationSeconds * m_Format.SamplePerSecond));
    }

    auto Metadata::GetTotalDurations() const -> double
    {
        return m_DurationSeconds;
    }

} // namespace N503::Audio::Codec
