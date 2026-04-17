#include "Pch.hpp"
#include "SourceVoice.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "../Frames/Buffer.hpp"
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/result_macros.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <mmreg.h>
#include <strsafe.h>
#include <xaudio2.h>

// 6. C++ Standard Libraries
#include <cstdint>
#include <format>

namespace N503::Audio::Device
{

    /// @brief フォーマットを元に XAudio2SourceVoice を生成
    SourceVoice::SourceVoice(Context* context, const Audio::Format& format) : m_Format(format)
    {
        auto pcmFormat = m_Format.ToRawFormat();
        // XAudio2 側にソースボイスを作成依頼。自身(this)をコールバックハンドラとして渡す。
        THROW_IF_FAILED(context->GetXAudio2()->CreateSourceVoice(&m_SourceVoice, reinterpret_cast<WAVEFORMATEX*>(&pcmFormat), 0, XAUDIO2_DEFAULT_FREQ_RATIO, this, nullptr, nullptr));
    }

    /// @brief 明示的なリソース破棄
    SourceVoice::~SourceVoice() noexcept
    {
        if (m_SourceVoice)
        {
            m_SourceVoice->DestroyVoice();
            m_SourceVoice = nullptr;
        }
    }

    /// @brief 再生開始処理
    bool SourceVoice::Start()
    {
        if (!m_SourceVoice)
        {
            return false;
        }
        THROW_IF_FAILED(m_SourceVoice->Start(0));
        m_IsPlaying = true;
        return true;
    }

    /// @brief 再生停止処理
    bool SourceVoice::Stop()
    {
        if (!m_SourceVoice)
        {
            return false;
        }
        THROW_IF_FAILED(m_SourceVoice->Stop(0));
        m_IsPlaying = false;
        return true;
    }

    /// @brief キューにあるバッファのフラッシュ
    bool SourceVoice::Flush()
    {
        if (!m_SourceVoice)
        {
            return false;
        }
        THROW_IF_FAILED(m_SourceVoice->FlushSourceBuffers());
        return true;
    }

    /// @brief XAudio2 へのバッファ送信処理
    auto SourceVoice::Submit(const Frames::Buffer& buffer, void* pBufferContext) -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }

        // フォーマットの BlockAlign に基づいて、全フレーム分のバイト数を計算
        const std::uint64_t bytesFromFrames = static_cast<std::uint64_t>(buffer.Count) * m_Format.BlockAlign;

        // 計算したバイト数とバッファ自体の Size が不一致な場合は警告をログに出す
        if (buffer.Size != bytesFromFrames)
        {
#ifdef _DEBUG
            Audio::Engine::Instance().GetDiagnostics().AddEntry({ Diagnostics::Severity::Warning, std::format("[SourceVoice::Submit] 注意: decoder が報告する Size({}) と FrameCount*BlockAlign({}) が不一致。FrameCount ベースを採用します", buffer.Size, bytesFromFrames).data() });
#endif
        }

        std::uint64_t audioBytes64 = bytesFromFrames;

        // XAudio2 の API 制限(UINT32)を超えないかチェック
        if (audioBytes64 > UINT32_MAX)
        {
#ifdef _DEBUG
            Audio::Engine::Instance().GetDiagnostics().AddEntry({ Diagnostics::Severity::Warning, std::format("[SourceVoice::Submit] 注意: AudioBytes が UINT32_MAX を超過 frames={} blockAlign={} bytes64={}", buffer.Count, m_Format.BlockAlign, audioBytes64).data() });
#endif
            audioBytes64 = UINT32_MAX;
        }

        const UINT32 audioBytes = static_cast<UINT32>(audioBytes64);

        // XAUDIO2_BUFFER 構造体のセットアップ
        XAUDIO2_BUFFER xBuffer = {};
        xBuffer.Flags = 0;
        xBuffer.AudioBytes = audioBytes;
        xBuffer.pAudioData = reinterpret_cast<const BYTE*>(buffer.Bytes);
        xBuffer.pContext = pBufferContext; // コールバックで使用されるコンテキスト

        // ストリームの終端である場合、終了フラグを立てる
        if (buffer.IsEndOfStream)
        {
            xBuffer.Flags |= XAUDIO2_END_OF_STREAM;
        }

        // 送信直前のログ記録
#ifdef _DEBUG
        Audio::Engine::Instance().GetDiagnostics().AddEntry({ Diagnostics::Severity::Info, std::format("[SourceVoice::Submit] SubmitSourceBuffer AudioBytes={} pAudioData={} pCtx={} eos={}", xBuffer.AudioBytes, static_cast<const void*>(xBuffer.pAudioData), xBuffer.pContext, buffer.IsEndOfStream ? 1 : 0).data() });
#endif
        // XAudio2 ボイスにデータを投入
        if (FAILED(m_SourceVoice->SubmitSourceBuffer(&xBuffer)))
        {
            return false;
        }

        return true;
    }

    /// @brief フォーマット取得
    auto SourceVoice::GetFormat() const noexcept -> const Audio::Format&
    {
        return m_Format;
    }

    /// @brief 現在のボリュームを設定します
    auto SourceVoice::SetVolume(float volume) -> bool
    {
        if (!m_SourceVoice)
        {
            return false;
        }
        m_SourceVoice->SetVolume(volume);
        return true;
    }

    /// @brief 現在のボリュームを取得します
    auto SourceVoice::GetVolume() const noexcept -> float
    {
        if (!m_SourceVoice)
        {
            return false;
        }
        float volume{ 0.0f };
        m_SourceVoice->GetVolume(&volume);
        return volume;
    }

    /// @brief キューに存在するバッファ数を取得します。
    auto SourceVoice::GetBuffersQueued() const noexcept -> std::uint32_t
    {
        XAUDIO2_VOICE_STATE state{};
        m_SourceVoice->GetState(&state);
        return state.BuffersQueued;
    }

} // namespace N503::Audio::Device
