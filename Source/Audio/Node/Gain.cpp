#include "Pch.hpp"
#include "Gain.hpp"

// 1. Project Headers
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Status.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <chrono>
#include <cstdlib>

namespace N503::Audio::Node
{

    Gain::Gain()
    {
    }

    auto Gain::OnPlay() -> bool
    {
        m_LastTime = std::chrono::steady_clock::now();
        return true;
    }

    auto Gain::OnStop() -> void
    {
    }

    auto Gain::Update(Context& context) -> bool
    {
        // --- Time.Delta の計測（仮実装、後で Time ノードへ移行予定）---
        const auto now = std::chrono::steady_clock::now();
        context.Time.Delta = now - m_LastTime;
        context.Time.Elapsed += context.Time.Delta;
        m_LastTime = now;
        // ---------------------------------------------------------------

        auto& fade = context.Effect.Fade;

        // フェードが不要な場合（Threshold が 0）はボリュームを 1.0 に固定して即終了
        if (fade.Threshold <= std::chrono::microseconds{ 0 })
        {
            context.Descriptor.Volume = 1.0f;
            return true;
        }

        // Delta を Direction の符号に合わせて Elapsed に加算する
        const auto delta = std::chrono::duration_cast<std::chrono::microseconds>(context.Time.Delta);

        if (fade.Direction > std::chrono::microseconds{ 0 })
        {
            fade.Elapsed += delta;
        }
        else if (fade.Direction < std::chrono::microseconds{ 0 })
        {
            fade.Elapsed -= delta;
        }

        // [0, Threshold] にクランプ
        fade.Elapsed = std::clamp(fade.Elapsed, std::chrono::microseconds{ 0 }, fade.Threshold);

        // 正規化された音量を計算 (0.0f 〜 1.0f)
        const float t = static_cast<float>(fade.Elapsed.count()) /
                        static_cast<float>(fade.Threshold.count());

        context.Descriptor.Volume = t;

        // フェードアウト完了（Elapsed が 0 まで下がりきった）
        if (fade.Direction < std::chrono::microseconds{ 0 } && fade.Elapsed <= std::chrono::microseconds{ 0 })
        {
            context.Descriptor.Volume = 0.0f;
            return true; // 再生処理を終了する
        }

        // フェードイン完了（Elapsed が Threshold まで上がりきった）
        if (fade.Direction > std::chrono::microseconds{ 0 } && fade.Elapsed >= fade.Threshold)
        {
            context.Descriptor.Volume = 1.0f;
            fade.Direction = std::chrono::microseconds{ 0 }; // 以降は維持
        }

        return true;
    }

} // namespace N503::Audio::Node
