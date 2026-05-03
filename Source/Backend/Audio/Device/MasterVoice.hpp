#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <xaudio2.h>

// 6. C++ Standard Libraries
#include <memory>

namespace N503::Audio::Device
{

    class SourceVoice;

    class SourceVoiceCache;

} // namespace N503::Audio::Device

namespace N503::Audio::Device
{

    class MasterVoice final
    {
    public:
        MasterVoice();

        ~MasterVoice();

        MasterVoice(const MasterVoice&) = delete;

        auto operator=(const MasterVoice&) -> MasterVoice& = delete;

        MasterVoice(MasterVoice&&) = delete;

        auto operator=(MasterVoice&&) -> MasterVoice& = delete;

    public:
        auto AcquireSourceVoice(const Format& format) -> SourceVoice*;

        auto ReleaseSourceVoice(SourceVoice* voice) -> void;

    public:
        auto GetXAudio2() -> IXAudio2*
        {
            return m_XAudio2.get();
        }

        auto GetSourceVoiceCache() -> SourceVoiceCache&
        {
            return *m_SourceVoiceCache;
        }

    private:
        wil::com_ptr<IXAudio2> m_XAudio2;

        IXAudio2MasteringVoice* m_MasteringVoice = nullptr;

        std::unique_ptr<SourceVoiceCache> m_SourceVoiceCache;
    };

} // namespace N503::Audio::Device
