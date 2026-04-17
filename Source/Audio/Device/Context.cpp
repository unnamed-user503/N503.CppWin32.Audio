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
        // 1. XAudio2 エンジンインスタンスの生成
        THROW_IF_FAILED(::XAudio2Create(m_XAudio2.put(), 0, XAUDIO2_DEFAULT_PROCESSOR));

        // 2. マスターリングボイス（デフォルトの出力デバイス）の作成
        THROW_IF_FAILED(m_XAudio2->CreateMasteringVoice(&m_MasteringVoice));

        // 3. デフォルトボリュームの設定
        THROW_IF_FAILED(m_MasteringVoice->SetVolume(1.0f));

        // 4. ボイスプールを自身のコンテキストを渡して生成
        m_SourceVoicePool = std::make_unique<SourceVoicePool>(this);
    }

    /// @brief コンテキストの終了処理
    Context::~Context()
    {
        // 1. まずプールを破棄し、管理下の全 SourceVoice を解体させる
        m_SourceVoicePool.reset();

        // 2. マスターリングボイスを明示的に破棄
        if (m_MasteringVoice)
        {
            m_MasteringVoice->DestroyVoice();
            m_MasteringVoice = nullptr;
        }

        // 3. XAudio2 エンジンを解放
        m_XAudio2.reset();
    }

    /// @brief プールからボイスを借用
    auto Context::AcquireSourceVoice(const Format& format) -> SourceVoice*
    {
        return m_SourceVoicePool->Borrow(format);
    }

    /// @brief プールへボイスを返却
    auto Context::ReleaseSourceVoice(SourceVoice* voice) -> void
    {
        m_SourceVoicePool->Return(voice);
    }

} // namespace N503::Audio::Device
