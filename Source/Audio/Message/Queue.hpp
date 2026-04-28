#pragma once

// 1. Project Headers
#include "Envelope.hpp"
#include "Packet.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Pool.hpp>
#include <N503/Memory/StorageAllocator.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <array>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <list>
#include <mutex>
#include <queue>
#include <vector>

namespace N503::Audio::Message
{

    class Queue final
    {
        static const std::size_t MaxMessageQueued = 4096;

        static const std::size_t BufferCount = 3;

    public:
        using Element = Envelope;

        struct Slot
        {
            alignas(void*) std::byte data[sizeof(Element) + 32];
        };

        using Storage = ::N503::Memory::Storage::Pool<Slot>;

        using Allocator = ::N503::Memory::StorageAllocator<Element, Storage>;

        using Strategy = std::list<Element, Allocator>;

        using Container = std::queue<Element, Strategy>;

    public:
        Queue() = default;

        Queue(const Queue&) = delete;

        auto operator=(const Queue&) -> Queue& = delete;

        Queue(Queue&&) = delete;

        auto operator=(Queue&&) -> Queue& = delete;

    public:
        auto Enqueue(Packet&&) -> void;

        auto EnqueueSync(Packet&&) -> void;

        auto Enqueue(std::vector<Packet>&&) -> void;

        auto EnqueueSync(std::vector<Packet>&&) -> void;

        [[nodiscard]]
        auto PopAll() -> Container;

        [[nodiscard]]
        auto GetWakeupEventHandle() const -> HANDLE;

        [[nodiscard]]
        auto HasCongestion(const float threshold = 0.8f) const noexcept -> bool;

    private:
        struct Buffer
        {
            Queue::Storage Storage{ MaxMessageQueued }; ///< Allocator より先に宣言する必要がある事に注意

            Queue::Allocator Allocator{ &Storage }; ///< Container より先に宣言する必要がある事に注意

            Queue::Container Container{ Allocator };
        };

        std::array<Buffer, BufferCount> m_Buffer{};

        std::uint32_t m_BufferIndex{ 0 };

        std::condition_variable m_BufferExchanged;

        wil::unique_event_nothrow m_WakeupEvent{ ::CreateEventW(nullptr, FALSE, FALSE, nullptr) };

        std::mutex m_Mutex;
    };

} // namespace N503::Renderer2D::Message
