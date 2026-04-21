#include "Pch.hpp"
#include "StreamSelector.hpp"

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <mfapi.h>
#include <mfobjects.h>
#include <mfreadwrite.h>

// 6. C++ Standard Libraries

namespace N503::Audio::Codec
{

    StreamSelector::StreamSelector(const wil::com_ptr<IMFSourceReader> &reader) : m_SourceReader(reader)
    {
    }

    void StreamSelector::SelectFirst()
    {
        wil::com_ptr<IMFMediaType> mfMediaType;
        THROW_IF_FAILED(::MFCreateMediaType(mfMediaType.put()));

        THROW_IF_FAILED(mfMediaType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio));
        THROW_IF_FAILED(mfMediaType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_Float));

        THROW_IF_FAILED(mfMediaType->SetUINT32(MF_MT_AUDIO_SAMPLES_PER_SECOND, Audio::Format::Default().SamplePerSecond)
        );
        THROW_IF_FAILED(mfMediaType->SetUINT32(MF_MT_AUDIO_NUM_CHANNELS, Audio::Format::Default().Channels));
        THROW_IF_FAILED(mfMediaType->SetUINT32(MF_MT_AUDIO_BITS_PER_SAMPLE, 32));

        THROW_IF_FAILED(
            m_SourceReader->SetCurrentMediaType(MF_SOURCE_READER_FIRST_AUDIO_STREAM, nullptr, mfMediaType.get())
        );
    }

} // namespace N503::Audio::Codec
