#include "Pch.hpp"
#include "Endpoint.hpp"

// 1. Project Headers
#include "../Device/SourceVoice.hpp"
#include "../Engine.hpp"
#include "../Frames/Buffer.hpp"
#include "Context.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Status.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>
#include <chrono>

// 6. C++ Standard Libraries

namespace N503::Audio::Node
{

    Endpoint::Endpoint(Device::SourceVoice* sourceVoice) : m_SourceVoice{ sourceVoice }
    {
    }

    auto Endpoint::OnPlay(const Audio::Format& format) -> void
    {
#ifdef _DEBUG
        Audio::Engine::Instance().GetDiagnosticsSink().AddEntry("[Endpoint::OnPlay] AcquireSourceVoice");
#endif
        if (!m_SourceVoice)
        {
            m_SourceVoice = Audio::Engine::Instance().GetDeviceContext().AcquireSourceVoice(format);
            m_SourceVoice->Start();
        }
    }

    auto Endpoint::OnStop() -> void
    {
#ifdef _DEBUG
        Audio::Engine::Instance().GetDiagnosticsSink().AddEntry("[Endpoint::OnStop] ReleaseSourceVoice");
#endif
        if (m_SourceVoice)
        {
            m_SourceVoice->Stop();
            m_SourceVoice->Flush();

            Audio::Engine::Instance().GetDeviceContext().ReleaseSourceVoice(m_SourceVoice);
            m_SourceVoice = nullptr;
        }
    }

    auto Endpoint::Update(Context& context) -> bool
    {
        if (!m_SourceVoice)
        {
            return true;
        }

        //if (context.Descriptor.Status == Audio::Status::Paused)
        //{
        //    //m_SourceVoice->Stop();
        //    return false; // 再生処理を継続する
        //}

        //if (context.Descriptor.Status == Audio::Status::Stopping)
        //{
        //    if (m_SourceVoice->GetBuffersQueued() == 0)
        //    {
        //        return true;
        //    }

        //    return false; // 再生処理を継続する
        //}
        if (context.Descriptor.Status == Audio::Status::Stopping)
        {
            // フェード時間が 0 (即時切断) の場合
            if (context.Effect.Fade.Threshold == std::chrono::microseconds(0))
            {
                m_SourceVoice->Stop();   // 鳴らすのを止める
                m_SourceVoice->Flush();  // キューにあるバッファを全部捨てる
            }
            else
            {
                // フェードアウトさせる場合、XAudio2 が動いている必要がある
                m_SourceVoice->Start();
            }

            // Flush されたか、鳴りきればここを通過してノードが解体される
            if (m_SourceVoice->GetBuffersQueued() == 0)
            {
                return true;
            }

            return false;
        }

        // 再生再開（Pause から Playing に戻った瞬間のケア）
        // Playing かつ XAudio2 が止まっていたら Start を呼ぶ
        // XAudio2 の Start/Stop は何度呼んでも安全です
        if (context.Descriptor.Status == Audio::Status::Playing)
        {
            m_SourceVoice->Start();
        }

        // 3. 通常再生中のデータ提出チェック
        // 送信すべきデータがない場合でも、Stopping でない限りは 現状を維持する
        if (!context.Buffers.Submit || context.Buffers.Submit->Status != Node::Entry::Status::Pending)
        {
            return false; // 再生処理を継続する
        }

        // ボリューム同期 (Effect ノードの結果を反映)
        m_SourceVoice->SetVolume(context.Descriptor.Volume);

        // 5. 統計・位置情報の更新
        context.Statistics.TotalSubmittedFrame += context.Buffers.Submit->Frames->Count;
        context.Statistics.TotalSubmittedCount++;
        context.Position.Current += context.Buffers.Submit->Frames->Count;

        // XAudio2へのSubmit時に使用するコンテキスト(同期フラグ)を用意
        void* pContext = nullptr;
        if (context.Buffers.Submit->Signal)
        {
            pContext = static_cast<void*>(context.Buffers.Submit->Signal);
        }

        // SubmitSourceBuffer を実行
        m_SourceVoice->Submit(*context.Buffers.Submit->Frames, pContext);

        // 送信完了したのでステータスを更新
        context.Buffers.Submit->Status = Node::Entry::Status::Submitted;

        return false; // 再生処理を継続する
    }

} // namespace N503::Audio::Node
