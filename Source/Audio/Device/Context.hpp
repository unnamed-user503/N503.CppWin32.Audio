#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

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

    class SourceVoicePool;

} // namespace N503::Audio::Device

namespace N503::Audio::Device
{

    class Context final
    {
    public:
        Context();

        ~Context();

        Context(const Context&) = delete;

        auto operator=(const Context&) -> Context& = delete;

        Context(Context&&) = delete;

        auto operator=(Context&&) -> Context& = delete;

    public:
        auto AcquireSourceVoice(const Format& format) -> SourceVoice*;

        auto ReleaseSourceVoice(SourceVoice* voice) -> void;

    public:
        auto GetXAudio2() -> IXAudio2*
        {
            return m_XAudio2.get();
        }

        auto GetSourceVoicePool() -> SourceVoicePool&
        {
            return *m_SourceVoicePool;
        }

    private:
        wil::com_ptr<IXAudio2> m_XAudio2;

        IXAudio2MasteringVoice* m_MasteringVoice = nullptr;

        std::unique_ptr<SourceVoicePool> m_SourceVoicePool;
    };

} // namespace N503::Audio::Device
