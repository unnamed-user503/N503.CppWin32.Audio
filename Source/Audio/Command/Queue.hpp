#pragma once

// 1. Project Headers
#include "Envelope.hpp"
#include "Packets/Packet.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Pool.hpp>
#include <N503/Memory/StorageAllocator.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <cstddef>
#include <list>
#include <mutex>
#include <queue>

namespace N503::Audio::Command
{

    class Queue final
    {
        static constexpr std::size_t MaxCommandsQueue = 4096;

    public:
        using Element = Command::Envelope;

        struct Slot
        {
            alignas(void*) std::byte data[sizeof(Element) + 32];
        };

        using Storage = N503::Memory::Storage::Pool<Slot>;

        using Allocator = N503::Memory::StorageAllocator<Element, Storage>;

        using Strategy = std::list<Element, Allocator>;

        using Container = std::queue<Element, Strategy>;

    public:
        Queue() = default;

        Queue(const Queue&) = delete;

        auto operator=(const Queue&) -> Queue& = delete;

    public:
        auto Push(Packets::Packet&&) -> void;

        auto PushSync(Packets::Packet&&) -> void;

        [[nodiscard]]
        auto PopAll() -> Container;

        [[nodiscard]]
        auto IsEmpty() -> bool;

        [[nodiscard]]
        auto GetWakeupEventHandle() const -> HANDLE;

    public:
        Storage m_Storage{ MaxCommandsQueue }; ///< Allocator / Container より先に宣言する必要がある事に注意

        Allocator m_Allocator{ &m_Storage };

        Container m_Container{ m_Allocator };

        wil::unique_event_nothrow m_WakeupEvent{ ::CreateEventA(nullptr, FALSE, FALSE, nullptr) };

        std::mutex m_Mutex;
    };

} // namespace N503::Audio::Command
