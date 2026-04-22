#pragma once

// 1. Project Headers
#include "../Frames/Buffer.hpp"
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <xaudio2.h>

// 6. C++ Standard Libraries
#include <atomic>
#include <cstdint>

namespace N503::Audio::Device
{

    class SourceVoice : public IXAudio2VoiceCallback
    {
    public:
        SourceVoice(Context* context, const Audio::Format& format);

        virtual ~SourceVoice() noexcept;

        auto Start() -> bool;

        auto Stop() -> bool;

        auto StopAndWait() -> bool;

        auto Flush() -> bool;

        auto Submit(const Frames::Buffer& buffer, void* pBufferContext) -> bool;

        auto GetFormat() const noexcept -> const Audio::Format&;

        auto SetVolume(float volume) -> bool;

        auto GetVolume() const noexcept -> float;

        auto GetBuffersQueued() const noexcept -> std::uint32_t;

        void STDMETHODCALLTYPE OnVoiceProcessingPassStart(UINT32 BytesRequired) override
        {
        }

        void STDMETHODCALLTYPE OnVoiceProcessingPassEnd() override
        {
        }

        void STDMETHODCALLTYPE OnStreamEnd() override
        {
        }

        void STDMETHODCALLTYPE OnBufferStart(void* pBufferContext) override;

        void STDMETHODCALLTYPE OnBufferEnd(void* pBufferContext) override;

        void STDMETHODCALLTYPE OnLoopEnd(void* pBufferContext) override;

        void STDMETHODCALLTYPE OnVoiceError(void* pBufferContext, HRESULT Error) override
        {
        }

    private:
        IXAudio2SourceVoice* m_SourceVoice{ nullptr };

        N503::Audio::Format m_Format{};

        wil::unique_event m_StoppedEvent{ ::CreateEventW(nullptr, TRUE, FALSE, nullptr) };

        std::atomic<std::uint64_t> m_PendingBuffers{ 0 };
    };

} // namespace N503::Audio::Device
