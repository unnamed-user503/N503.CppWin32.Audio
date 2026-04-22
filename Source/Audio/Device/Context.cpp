#include "Pch.hpp"
#include "Context.hpp"

// 1. Project Headers
#include "SourceVoice.hpp"
#include "SourceVoicePool.hpp"

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

    /// @brief コンテキストの初期化処理
    Context::Context()
    {
        THROW_IF_FAILED(::XAudio2Create(m_XAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR));
        THROW_IF_FAILED(m_XAudio2->CreateMasteringVoice(&m_MasteringVoice));
        THROW_IF_FAILED(m_MasteringVoice->SetVolume(1.0f));

        m_SourceVoicePool = std::make_unique<SourceVoicePool>(this);
    }

    Context::~Context()
    {
        m_SourceVoicePool.reset();

        if (m_MasteringVoice)
        {
            m_MasteringVoice->DestroyVoice();
            m_MasteringVoice = nullptr;
        }

        m_XAudio2.reset();
    }

    auto Context::AcquireSourceVoice(const Format& format) -> SourceVoice*
    {
        return m_SourceVoicePool->Borrow(format);
    }

    auto Context::ReleaseSourceVoice(SourceVoice* voice) -> void
    {
        m_SourceVoicePool->Return(voice);
    }

} // namespace N503::Audio::Device
