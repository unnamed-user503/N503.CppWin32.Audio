#include "Pch.hpp"
#include "SourceVoice.hpp"

// 1. Project Headers
#include "Signal.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <atomic>

namespace N503::Audio::Device
{

    /// @brief バッファの再生が開始された際のコールバック
    /// @param pBufferContext Submit 時に渡されたコンテキストポインタ（Signal 構造体であることを期待）
    void WINAPI SourceVoice::OnBufferStart(void* pBufferContext)
    {
        if (pBufferContext)
        {
            // コンテキストを Signal ポインタとして解釈し、イベント情報を書き込む
            auto* signal = reinterpret_cast<Device::Signal*>(pBufferContext);
            signal->Event.store(Device::Signal::Event::BufferStart, std::memory_order_relaxed);
            // 通知フラグを立てる（Release セマンティクスにより、それ以前の書き込みを可視にする）
            signal->Notify.store(true, std::memory_order_release);
        }
    }

    /// @brief バッファの再生が完了した際のコールバック
    /// @param pBufferContext Submit 時に渡されたコンテキストポインタ
    void WINAPI SourceVoice::OnBufferEnd(void* pBufferContext)
    {
        if (pBufferContext)
        {
            auto* signal = reinterpret_cast<Device::Signal*>(pBufferContext);
            signal->Event.store(Device::Signal::Event::BufferEnd, std::memory_order_relaxed);
            signal->Notify.store(true, std::memory_order_release);
        }

        // 保留中バッファのカウンタを1つ減らす
        // 最後のバッファになったら終了イベントの旗を立てる
        if (m_PendingBuffers.fetch_sub(1, std::memory_order_acquire) == 1)
        {
            ::SetEvent(m_StoppedEvent.get());
        }
    }

    /// @brief ループ領域の終端に達した際のコールバック
    /// @param pBufferContext Submit 時に渡されたコンテキストポインタ
    /// @details 現在の実装では BufferEnd と同様の扱いとして通知します。
    void WINAPI SourceVoice::OnLoopEnd(void* pBufferContext)
    {
        if (pBufferContext)
        {
            auto* signal = reinterpret_cast<Device::Signal*>(pBufferContext);
            signal->Event.store(Device::Signal::Event::BufferEnd, std::memory_order_relaxed);
            signal->Notify.store(true, std::memory_order_release);
        }
    }

} // namespace N503::Audio::Device
