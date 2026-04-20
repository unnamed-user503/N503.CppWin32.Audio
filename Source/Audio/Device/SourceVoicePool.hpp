#pragma once

// 1. Project Headers
#include "SourceVoice.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Memory/Storage/Pool.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <vector>

namespace N503::Audio::Device
{

    /// @brief オーディオエンジン全体のコンテキストを管理するクラスの先行宣言
    class Context;

} // namespace N503::Audio::Device

namespace N503::Audio::Device
{

    /// @brief IXAudio2SourceVoice を管理するオブジェクトプールクラス
    /// @details 音声再生インスタンスの頻繁な生成・破棄によるオーバーヘッドを抑制し、
    ///          同一フォーマットのボイスを効率的に再利用します。
    class SourceVoicePool final
    {
    public:
        /// @brief プールが保持・管理できる最大ボイス数
        constexpr static std::uint32_t MaxAvailableSourceVoices = 64;

    public:
        /// @brief コンストラクタ
        /// @param context 親となるオーディオコンテキスト
        explicit SourceVoicePool(Context* context);

        /// @brief デストラクタ
        /// @details 管理している全ての SourceVoice インスタンスを破棄します。
        ~SourceVoicePool();

        /// @brief 指定されたフォーマットに合致する SourceVoice を取得します
        /// @param format 要求するオーディオフォーマット
        /// @return 利用可能な SourceVoice ポインタ。取得できない場合は nullptr。
        auto Borrow(const Format& format) -> SourceVoice*;

        /// @brief 使用済みの SourceVoice をプールに返却します
        /// @param voice 返却するインスタンス
        auto Return(SourceVoice* voice) -> void;

    private:
        /// @brief SourceVoice インスタンスのメモリ実体を管理するプール
        N503::Memory::Storage::Pool<SourceVoice> m_Storage{ MaxAvailableSourceVoices };

        /// @brief 親コンテキストへのポインタ
        Context* m_Context{ nullptr };

        /// @brief 現在貸し出し可能な（フリーな）SourceVoice のポインタリスト
        std::vector<SourceVoice*> m_Availables;

        /// @brief 生成された全 SourceVoice の生存を追跡するためのインデックスリスト
        std::vector<SourceVoice*> m_Indexes;
    };

} // namespace N503::Audio::Device
