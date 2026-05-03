#include "Pch.hpp"
#include "SourceVoice.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "../Frames/Buffer.hpp"
#include "MasterVoice.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Diagnostics/Reporter.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <mmreg.h>
#include <strsafe.h>
#include <xaudio2.h>

// 6. C++ Standard Libraries
#include <chrono>
#include <cstdint>
#include <format>
#include <thread>

namespace N503::Audio::Device
{

    SourceVoice::SourceVoice(MasterVoice* masterVoice, const Audio::Format& format) : m_Format(format)
    {
        auto pcmFormat = m_Format.ToRawFormat();
        auto rawFormat = reinterpret_cast<WAVEFORMATEX*>(&pcmFormat);

        THROW_IF_FAILED(masterVoice->GetXAudio2()->CreateSourceVoice(&m_SourceVoice, rawFormat, 0, XAUDIO2_DEFAULT_FREQ_RATIO, this, nullptr, nullptr));
    }

    SourceVoice::~SourceVoice() noexcept
    {
        if (m_SourceVoice)
        {
            StopAndWait();
            m_SourceVoice->DestroyVoice();
            m_SourceVoice = nullptr;
        }
    }

    auto SourceVoice::Start() -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }

        THROW_IF_FAILED(m_SourceVoice->Start(0));
        return true;
    }

    auto SourceVoice::Stop() -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }

        THROW_IF_FAILED(m_SourceVoice->Stop(0));
        return true;
    }

    auto SourceVoice::StopAndWait() -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }

        THROW_IF_FAILED(m_SourceVoice->Stop(0));
        THROW_IF_FAILED(m_SourceVoice->FlushSourceBuffers());

        while (m_PendingBuffers.load(std::memory_order_acquire) > 0)
        {
            ::WaitForSingleObject(m_StoppedEvent.get(), INFINITE);
        }

        return true;
    }

    auto SourceVoice::Flush() -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }

        THROW_IF_FAILED(m_SourceVoice->FlushSourceBuffers());
        return true;
    }

    auto SourceVoice::Submit(const Frames::Buffer& buffer, void* pBufferContext) -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }

        // 保留中のバッファのカウンタを1つ増やす
        m_PendingBuffers.fetch_add(1, std::memory_order_relaxed);
        // 停止イベントをリセット
        m_StoppedEvent.ResetEvent();

        // XAUDIO2_BUFFER 構造体のセットアップ
        XAUDIO2_BUFFER sourceBuffer = {};
        sourceBuffer.Flags          = 0;
        sourceBuffer.AudioBytes     = static_cast<UINT32>(buffer.Count * m_Format.BlockAlign);
        sourceBuffer.pAudioData     = reinterpret_cast<const BYTE*>(buffer.Bytes.data());
        sourceBuffer.pContext       = pBufferContext; // コールバックで使用されるコンテキスト

        // ストリームの終端である場合、終了フラグを立てる
        if (buffer.IsEndOfStream)
        {
            sourceBuffer.Flags |= XAUDIO2_END_OF_STREAM;
        }

        // 送信直前のログ記録
#if 0
        const auto log = std::format(
            "[SourceVoice] SubmitSourceBuffer AudioBytes={} pAudioData={} pContext={} EndOfStream={}",
            sourceBuffer.AudioBytes,
            static_cast<const void*>(sourceBuffer.pAudioData),
            sourceBuffer.pContext,
            buffer.IsEndOfStream ? "o" : "x"
        );

        Audio::Engine::GetInstance().GetDiagnosticsReporter().Verbose(log);
#endif
        // XAudio2 ボイスにデータを投入
        if (FAILED(m_SourceVoice->SubmitSourceBuffer(&sourceBuffer)))
        {
            m_PendingBuffers.fetch_sub(1, std::memory_order_relaxed);
            return false;
        }

        return true;
    }

    auto SourceVoice::GetFormat() const noexcept -> const Audio::Format&
    {
        return m_Format;
    }

    auto SourceVoice::SetVolume(float volume) -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }

        m_SourceVoice->SetVolume(volume);
        return true;
    }

    auto SourceVoice::GetVolume() const noexcept -> float
    {
        if (!m_SourceVoice)
        {
            return 0.0f;
        }

        float volume{ 0.0f };
        m_SourceVoice->GetVolume(&volume);
        return volume;
    }

    auto SourceVoice::GetBuffersQueued() const noexcept -> std::uint32_t
    {
        XAUDIO2_VOICE_STATE state{};
        m_SourceVoice->GetState(&state);
        return state.BuffersQueued;
    }

} // namespace N503::Audio::Device
