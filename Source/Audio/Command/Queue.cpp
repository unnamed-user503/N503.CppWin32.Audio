#include "Pch.hpp"
#include "Queue.hpp"

// 1. Project Headers
#include "Envelope.hpp"
#include "Packets/Packet.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <queue>
#include <semaphore>
#include <utility>
#ifdef _DEBUG
#include <format>
#include <string>
#endif

namespace N503::Audio::Command
{

    auto Queue::Push(Packets::Packet&& packet) -> void
    {
#ifdef _DEBUG
        {
            const std::size_t currentSize = m_Container.size();
            const std::size_t capacity = MaxCommandsQueue;

            if (currentSize > capacity * 0.8)
            {
                ::OutputDebugStringA(std::format("CommandQueue is congesting! Current: {}, TypeIndex: {}\n", currentSize, packet.index()).data());
            }
        }
#endif
        {
            const std::lock_guard lock{ m_Mutex };

            m_Container.push({ .Packet = std::move(packet), .Signal = nullptr });
        }

        m_WakeupEvent.SetEvent();
    }

    /// @brief
    /// @param packet
    /// @return
    auto Queue::PushSync(Packets::Packet&& packet) -> void
    {
        std::binary_semaphore signal{ 0 };

#ifdef _DEBUG
        {
            const std::size_t currentSize = m_Container.size();
            const std::size_t capacity = MaxCommandsQueue;

            if (currentSize > capacity * 0.8)
            {
                ::OutputDebugStringA(std::format("CommandQueue is congesting! Current: {}, TypeIndex: {}\n", currentSize, packet.index()).data());
            }
        }
#endif
        {
            const std::lock_guard lock{ m_Mutex };

            m_Container.push({ .Packet = std::move(packet), .Signal = &signal });
        }

        m_WakeupEvent.SetEvent();

        signal.acquire();
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::PopAll() -> Container
    {
        const std::lock_guard lock{ m_Mutex };

        auto container = std::move(m_Container);

        m_Container = Container{ m_Allocator };

        return container;
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::IsEmpty() -> bool
    {
        return m_Container.empty();
    }

    /// @brief
    /// @return
    [[nodiscard]]
    auto Queue::GetWakeupEventHandle() const -> HANDLE
    {
        return m_WakeupEvent.get();
    }

} // namespace N503::Audio::Command
