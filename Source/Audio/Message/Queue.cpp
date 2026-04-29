#include "Pch.hpp"
#include "Queue.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "Envelope.hpp"
#include "Packet.hpp"

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <algorithm>
#include <format>
#include <iterator>
#include <mutex>
#include <semaphore>
#include <utility>
#include <vector>

namespace N503::Audio::Message
{

    auto Queue::Enqueue(Packet&& packet) -> void
    {
        {
            std::unique_lock lock(m_Mutex);

            if (HasCongestion())
            {
                m_BufferExchanged.wait(lock, [this] { return m_Buffer[m_BufferIndex].Container.size() < MaxMessageQueued; });
            }

            m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(packet) });
        }

        m_WakeupEvent.SetEvent();
    }

    auto Queue::EnqueueSync(Packet&& packet) -> void
    {
        std::binary_semaphore signal{ 0 };

        {
            std::unique_lock lock(m_Mutex);

            (void)HasCongestion();

            m_BufferExchanged.wait(lock, [this] { return m_Buffer[m_BufferIndex].Container.size() < MaxMessageQueued; });

            m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(packet), &signal });
        }

        m_WakeupEvent.SetEvent();

        signal.acquire();
    }

    auto Queue::Enqueue(std::vector<Packet>&& packets) -> void
    {
        const std::size_t total = packets.size();
        std::size_t sent        = 0;

        while (sent < total)
        {
            std::unique_lock lock(m_Mutex);

            m_BufferExchanged.wait(lock, [this] { return m_Buffer[m_BufferIndex].Container.size() < MaxMessageQueued; });

            const std::size_t current = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t remain  = (current < MaxMessageQueued) ? (MaxMessageQueued - current) : 0;
            const std::size_t submit  = std::min(total - sent, remain);

            if (submit > 0)
            {
                auto first = packets.begin() + sent;
                auto last  = first + submit;

                std::vector<Packet> bundle;
                bundle.reserve(submit);
                std::move(first, last, std::back_inserter(bundle));

                m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(bundle), nullptr });
                m_WakeupEvent.SetEvent();

                sent += submit;
            }
        }
    }

    auto Queue::EnqueueSync(std::vector<Packet>&& packets) -> void
    {
        std::binary_semaphore signal{ 0 };

        const std::size_t total = packets.size();
        std::size_t sent        = 0;

        while (sent < total)
        {
            std::unique_lock lock(m_Mutex);

            m_BufferExchanged.wait(lock, [this] { return m_Buffer[m_BufferIndex].Container.size() < MaxMessageQueued; });

            const std::size_t current = m_Buffer[m_BufferIndex].Container.size();
            const std::size_t remain  = (current < MaxMessageQueued) ? (MaxMessageQueued - current) : 0;
            const std::size_t submit  = std::min(total - sent, remain);

            if (submit > 0)
            {
                auto first = packets.begin() + sent;
                auto last  = first + submit;

                std::vector<Packet> bundle;
                bundle.reserve(submit);
                std::move(first, last, std::back_inserter(bundle));

                m_Buffer[m_BufferIndex].Container.push(Envelope{ std::move(bundle), &signal });
                m_WakeupEvent.SetEvent();

                sent += submit;
            }
        }

        signal.acquire();
    }

    [[nodiscard]]
    auto Queue::PopAll() -> Container
    {
        std::scoped_lock lock(m_Mutex);

        // 現在のバッファからコンテナをムーブして取得する
        auto container = std::move(m_Buffer[m_BufferIndex].Container);

        // 古いコンテナは破棄されるため、Allocator を再利用して新しいコンテナを構築する
        m_Buffer[m_BufferIndex].Container = Container{ m_Buffer[m_BufferIndex].Allocator };

        // 次のバッファに切り替える
        m_BufferIndex = (m_BufferIndex + 1) % m_Buffer.size();

        // バッファが切り替わったことを待機している可能性のある Enqueue 呼び出しに通知する
        m_BufferExchanged.notify_one();

        return container;
    }

    [[nodiscard]]
    auto Queue::GetWakeupEventHandle() const -> HANDLE
    {
        return m_WakeupEvent.get();
    }

    [[nodiscard]]
    auto Queue::HasCongestion(const float threshold) const noexcept -> bool
    {
#ifdef _DEBUG
        const std::size_t currentSize = m_Buffer[m_BufferIndex].Container.size();
        const std::size_t capacity    = MaxMessageQueued;
        const bool isCongested        = currentSize >= static_cast<std::size_t>(capacity * threshold);

        if (isCongested)
        {
            Engine::GetInstance().GetDiagnosticsReporter().Warning(std::format(L"EventQueue is congesting: CurrentSize={}, Capacity={}\n", currentSize, capacity));
        }
#endif
        return isCongested;
    }

} // namespace N503::Renderer2D::Message
