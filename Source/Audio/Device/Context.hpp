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

    /// @brief 再生インスタンスの先行宣言
    class SourceVoice;

    /// @brief ボイス管理プールの先行宣言
    class SourceVoicePool;

} // namespace N503::Audio::Device

namespace N503::Audio::Device
{

    /// @brief オーディオデバイスの実行状態とリソースを管理するコンテキストクラス
    /// @details XAudio2 エンジンの初期化、マスターボイスの保持、および
    ///          SourceVoice の生成・再利用（プール）を統括します。
    class Context final
    {
    public:
        /// @brief コンストラクタ
        /// @details XAudio2 インスタンスとマスターリングボイスを初期化します。
        Context();

        /// @brief デストラクタ
        /// @details プールおよび各 XAudio2 ボイスリソースを安全に破棄します。
        ~Context();

        // コピー・ムーブは禁止
        Context(const Context&) = delete;
        auto operator=(const Context&) -> Context& = delete;
        Context(Context&&) = delete;
        auto operator=(Context&&) -> Context& = delete;

    public:
        /// @brief 指定したフォーマットに適合するソースボイスを取得します
        /// @param format 要求するオーディオフォーマット
        /// @return 利用可能な SourceVoice へのポインタ
        auto AcquireSourceVoice(const Format& format) -> SourceVoice*;

        /// @brief 使用しなくなったソースボイスを返却します
        /// @param voice 返却する SourceVoice ポインタ
        auto ReleaseSourceVoice(SourceVoice* voice) -> void;

    public:
        /// @brief 内部の IXAudio2 オブジェクトへのポインタを取得します
        /// @return IXAudio2 インスタンスへのポインタ
        auto GetXAudio2() -> IXAudio2*
        {
            return m_XAudio2.get();
        }

        /// @brief 内部のボイスプールへの参照を取得します
        /// @return SourceVoicePool への参照
        auto GetSourceVoicePool() -> SourceVoicePool&
        {
            return *m_SourceVoicePool;
        }

    private:
        /// @brief XAudio2 エンジン本体（スマートポインタで管理）
        wil::com_ptr<IXAudio2> m_XAudio2;

        /// @brief 最終的な出力先となるマスターリングボイス
        IXAudio2MasteringVoice* m_MasteringVoice = nullptr;

        /// @brief ボイスの再利用を管理するプールオブジェクト
        std::unique_ptr<SourceVoicePool> m_SourceVoicePool;
    };

} // namespace N503::Audio::Device
