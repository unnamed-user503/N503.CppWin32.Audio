#pragma once

// 1. Project Headers
#include "../Device/Signal.hpp"
#include "../Frames/Buffer.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Node
{

    struct Entry final
    {
        /// @brief キューのスロット内のバッファへのポインタ
        Frames::Buffer *Frames{nullptr};

        /// @brief キューのスロット内のシグナルへのポインタ
        Device::Signal *Signal{nullptr};

        enum class Status
        {
            Empty = 0, ///< バッファが無い状態
            Ready,     ///< バッファの書き込み準備が出来ている状態
            Present,   ///< バッファに書き込まれた状態
            Pending,   ///< バッファが提出(Submit)されるのを待機している状態
            Submitted, ///< バッファが提出(Submit)された状態
            Playing,   ///< バッファは再生中の状態
            Completed, ///< バッファの使用が終わり掃除されるためのマークされた状態
        };

        /// @brief
        Status Status{Status::Empty};
    };

} // namespace N503::Audio::Node
