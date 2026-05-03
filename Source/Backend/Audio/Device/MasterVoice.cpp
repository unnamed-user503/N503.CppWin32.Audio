#include "Pch.hpp"
#include "MasterVoice.hpp"

// 1. Project Headers
#include "SourceVoice.hpp"
#include "SourceVoiceCache.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <xaudio2.h>

// 6. C++ Standard Libraries
#include <memory>

namespace N503::Audio::Device
{

    MasterVoice::MasterVoice()
    {
        THROW_IF_FAILED(::XAudio2Create(m_XAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR));
        THROW_IF_FAILED(m_XAudio2->CreateMasteringVoice(&m_MasteringVoice));
        THROW_IF_FAILED(m_MasteringVoice->SetVolume(1.0f));

        m_SourceVoiceCache = std::make_unique<SourceVoiceCache>(this);
    }

    MasterVoice::~MasterVoice()
    {
        m_SourceVoiceCache.reset();

        if (m_MasteringVoice)
        {
            m_MasteringVoice->DestroyVoice();
            m_MasteringVoice = nullptr;
        }

        m_XAudio2.reset();
    }

    auto MasterVoice::AcquireSourceVoice(const Format& format) -> SourceVoice*
    {
        return m_SourceVoiceCache->Borrow(format);
    }

    auto MasterVoice::ReleaseSourceVoice(SourceVoice* voice) -> void
    {
        m_SourceVoiceCache->Return(voice);
    }

} // namespace N503::Audio::Device
