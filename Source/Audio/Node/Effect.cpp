#include "Pch.hpp"
#include "Effect.hpp"

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

    Effect::Effect(Parameters parameters)
    {
    }

    auto Effect::Update(Context& context) -> bool
    {

        if (context.Descriptor.Status == Audio::Status::Paused)
        {
            return false;
        }

        auto& fade = context.Effect.Fade;

        // 1. 【最優先】全体の停止号令(Stopping)を確認
        // QueueがEOSを検知してStoppingになったら、フェード中であっても
        // 「音量が0（またはフェード設定なし）」なら即座に終了を報告する。
        if (context.Descriptor.Status == Audio::Status::Stopping)
        {
            // フェードアウト中（Direction > 0）かつ、まだ音量が残っている（Elapsed < Threshold）場合以外は終了
            const bool isBusyFadingOut = (fade.Threshold.count() > 0 && fade.Direction.count() > 0 && fade.Elapsed < fade.Threshold);

            if (!isBusyFadingOut)
            {
                return true; // これで Node[1]=o になり、Pathが解体されます
            }
        }

        // 2. フェード設定がない通常の再生継続
        if (fade.Threshold.count() <= 0)
        {
            return false;
        }

        // 3. フェード進行処理
        fade.Elapsed += fade.Direction;
        float progress = std::clamp(static_cast<float>(std::abs(fade.Elapsed.count())) / fade.Threshold.count(), 0.0f, 1.0f);

        const bool isFadeOut = fade.Direction.count() >= 0;
        context.Descriptor.Volume = isFadeOut ? (1.0f - progress) : progress;

        // 4. 完了判定（手動フェードアウト用）
        if (progress >= 1.0f)
        {
            if (isFadeOut)
            {
                // ボリュームを絞りきったら、自ら「停止処理(Stopping)」へ移行を促す
                if (context.Descriptor.Status == Audio::Status::Playing)
                {
                    context.Descriptor.Status = Audio::Status::Stopping;
                }
                return true;
            }

            // フェードイン完了後は、そのまま再生を続ける
            return false;
        }

        return false;
    }
} // namespace N503::Audio::Node
