#include "Pch.hpp"
#include "SourceVoicePool.hpp"

// 1. Project Headers
#include "Context.hpp"
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
    SourceVoicePool::SourceVoicePool(Context* context) : m_Context(context)
    {
        // 管理リストのメモリを事前に予約し、再アロケーションを抑制する
        m_Availables.reserve(MaxAvailableSourceVoices);
        m_Indexes.reserve(MaxAvailableSourceVoices);
    }

    /// @brief 全リソースのクリーンアップ
    SourceVoicePool::~SourceVoicePool()
    {
        // 索引(m_Indexes)を辿って、全てのボイスのデストラクタを明示的に呼び出す
        // これにより SourceVoice 内の XAudio2::DestroyVoice が確実に実行される
        for (auto* voice : m_Indexes)
        {
            std::destroy_at(voice);
        }
        // メモリ領域自体の解放は m_Storage (N503::Memory::Pool) の破棄時に一括で行われる
    }

    /// @brief ボイスの借用処理
    auto SourceVoicePool::Borrow(const Format& format) -> SourceVoice*
    {
        // 1. Availables（貸出可能リスト）から、要求されたフォーマットと一致するものを探す
        for (std::size_t i = 0; i < m_Availables.size(); ++i)
        {
            if (m_Availables[i]->GetFormat() == format)
            {
                SourceVoice* voice = m_Availables[i];

                // --- Swap-and-Pop テクニック ---
                // リストの途中の要素を削除すると後続のシフトで O(N) かかるため、
                // 末尾要素と入れ替えてから pop_back することで O(1) で削除を行う。
                if (i != m_Availables.size() - 1)
                {
                    m_Availables[i] = m_Availables.back();
                }

                m_Availables.pop_back();

                // 再利用開始時に再生を開始
                voice->Start();
                return voice;
            }
        }

        // 2. 一致する空きインスタンスがなく、最大数に達していなければ新規作成する
        if (m_Indexes.size() < MaxAvailableSourceVoices)
        {
            // Storage (Pool) からメモリを切り出し、インスタンスを構築
            if (SourceVoice* voice = m_Storage.Create(m_Context, format))
            {
                m_Indexes.push_back(voice);
                voice->Start();
                return voice;
            }
        }

        // 空きがなく、新規作成もできない場合
        return nullptr;
    }

    /// @brief ボイスの返却処理
    auto SourceVoicePool::Return(SourceVoice* voice) -> void
    {
        if (voice)
        {
            // 再生を停止し、貸出可能リストに追加する
            voice->Stop();
            m_Availables.push_back(voice);
        }
    }

} // namespace N503::Audio::Device
