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
        // 一時停止中は「まだ仕事の途中」なので false
        if (context.Descriptor.Status == Audio::Status::Paused)
        {
            return false;
        }

        auto& fade = context.Effect.Fade;

        // 外部から停止要求（Stopping）が来ている場合の処理
        if (context.Descriptor.Status == Audio::Status::Stopping)
        {
            // フェードアウト設定があるか確認
            const bool hasFadeOut        = (fade.Threshold.count() > 0 && fade.Direction.count() > 0);
            const bool isFadeOutComplete = (fade.Elapsed >= fade.Threshold);

            // フェードアウト中ならまだ仕事中、終わっているか設定がなければ「仕事完了」
            if (!hasFadeOut || isFadeOutComplete)
            {
                return true;
            }
        }

        // フェード設定がない場合
        if (fade.Threshold.count() <= 0)
        {
            // Stoppingでないなら通常再生中なので、仕事は終わっていない
            return false;
        }

        // フェード進捗の更新
        fade.Elapsed   += fade.Direction;
        float progress  = std::clamp(static_cast<float>(std::abs(fade.Elapsed.count())) / fade.Threshold.count(), 0.0f, 1.0f);

        const bool isFadeOut      = (fade.Direction.count() >= 0);
        context.Descriptor.Volume = isFadeOut ? (1.0f - progress) : progress;

        // フェード完了判定
        if (progress >= 1.0f)
        {
            if (isFadeOut)
            {
                // フェードアウトして無音になったら仕事完了
                context.Descriptor.Status = Audio::Status::Stopping;
                return true;
            }
            else
            {
                // フェードイン完了時は「通常再生」に移行するだけなので、仕事は続く
                fade.Threshold = std::chrono::microseconds(0);
                return false;
            }
        }

        return false; // まだフェード中なので仕事継続
    }

} // namespace N503::Audio::Node
