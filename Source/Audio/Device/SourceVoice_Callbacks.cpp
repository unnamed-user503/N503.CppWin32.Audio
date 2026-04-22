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

    void WINAPI SourceVoice::OnBufferStart(void* pBufferContext)
    {
        if (pBufferContext)
        {
            auto* signal = reinterpret_cast<Device::Signal*>(pBufferContext);
            signal->Event.store(Device::Signal::Event::BufferStart, std::memory_order_relaxed);
            signal->Notify.store(true, std::memory_order_release);
        }
    }

    void WINAPI SourceVoice::OnBufferEnd(void* pBufferContext)
    {
        if (pBufferContext)
        {
            auto* signal = reinterpret_cast<Device::Signal*>(pBufferContext);
            signal->Event.store(Device::Signal::Event::BufferEnd, std::memory_order_relaxed);
            signal->Notify.store(true, std::memory_order_release);
        }

        if (m_PendingBuffers.fetch_sub(1, std::memory_order_acquire) == 1)
        {
            ::SetEvent(m_StoppedEvent.get());
        }
    }

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
