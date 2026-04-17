#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <atomic>

namespace N503::Audio::Device
{

    /// @brief オーディオデバイスのイベント通知用構造体
    /// @details XAudio2
    /// のボイスコールバックから再生スレッドや管理クラスへ、再生状態の変化を通知するために使用されます。
    struct Signal
    {
        /// @brief 通知されるイベントの種類
        enum Event : int
        {
            None = 0,    ///< イベントなし
            BufferStart, ///< バッファの再生開始
            BufferEnd,   ///< バッファの再生完了（またはループ終了）
        };

        /// @brief 新しい通知が発生したかどうかを示すフラグ
        /// @note atomic 操作により、異なるスレッド間でも安全にフラグの読み書きが可能です。
        std::atomic<bool> Notify{ false };

        /// @brief 発生したイベントの具体的な種類
        /// @note コールバック側で書き込まれ、監視側で読み取られます。
        std::atomic<Event> Event{ Event::None };
    };

} // namespace N503::Audio::Device
