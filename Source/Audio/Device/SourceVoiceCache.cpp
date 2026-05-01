#include "Pch.hpp"
#include "SourceVoiceCache.hpp"

// 1. Project Headers
#include "MasterVoice.hpp"
#include "SourceVoice.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <memory>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Device
{
    /// @brief プールの初期化
    SourceVoiceCache::SourceVoiceCache(MasterVoice* masterVoice) : m_MasterVoice(masterVoice)
    {
        // 構築時に最大数までリザーブし、実行中の再確保をゼロにする
        m_Storage.reserve(MaxAvailableSourceVoices);
        m_Availables.reserve(MaxAvailableSourceVoices);
    }

    /// @brief 全リソースのクリーンアップ
    SourceVoiceCache::~SourceVoiceCache()
    {
        // unique_ptr が各 SourceVoice のデストラクタを自動で呼ぶため、
        // XAudio2 の DestroyVoice も安全に実行される。
        // 明示的なループ破棄は不要。
    }

    /// @brief ボイスの借用処理
    auto SourceVoiceCache::Borrow(const Format& format) -> SourceVoice*
    {
        // 1. Availables（貸出可能リスト）からフォーマットが一致するものを探す
        auto it = std::find_if(m_Availables.begin(), m_Availables.end(), [&format](SourceVoice* voice) { return voice->GetFormat() == format; });

        if (it != m_Availables.end())
        {
            SourceVoice* voice = *it;

            // Swap-and-Pop で O(1) 削除
            *it = m_Availables.back();
            m_Availables.pop_back();

            voice->Start();
            return voice;
        }

        // 2. 一致する空きがなく、最大数に達していなければ新規作成
        if (m_Storage.size() < MaxAvailableSourceVoices)
        {
            // unique_ptr で実体を生成。m_Storage が所有権を持つ
            auto voice       = std::make_unique<SourceVoice>(m_MasterVoice, format);
            SourceVoice* raw = voice.get();

            m_Storage.push_back(std::move(voice));

            raw->Start();
            return raw;
        }

        return nullptr;
    }

    /// @brief ボイスの返却処理
    auto SourceVoiceCache::Return(SourceVoice* voice) -> void
    {
        if (voice)
        {
            voice->Stop();
            voice->Flush();
            // ポインタのみを貸出可能リストに戻す
            m_Availables.push_back(voice);
        }
    }

} // namespace N503::Audio::Device
