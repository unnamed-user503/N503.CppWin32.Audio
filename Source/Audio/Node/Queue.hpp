#pragma once

// 1. Project Headers
#include "../Device/Signal.hpp"
#include "../Frames/Buffer.hpp"
#include "Context.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Memory/Storage/Queue.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>

namespace N503::Audio::Node
{

    class Queue
    {
    public:
        static constexpr std::size_t MaxBuffersQueue = 4;

        static constexpr std::size_t PrefetchBuffersQueue = 2;

    public:
        explicit Queue();

        auto OnPlay(const Audio::Format& format) -> bool;

        auto OnStop() -> void;

        auto Update(Context& context) -> bool;

    private:
        auto Sweep(Context& context) -> bool;

    private:
        std::unique_ptr<N503::Memory::Storage::Queue> m_Storage{};

        std::array<Node::Entry, MaxBuffersQueue> m_Entries{};

        std::array<Frames::Buffer, MaxBuffersQueue> m_Buffers{};

        std::array<Device::Signal, MaxBuffersQueue> m_Signals{};

        std::size_t m_BytesPerFrame{ 0 };

        std::size_t m_CurrentBufferSize{ 0 };
    };

} // namespace N503::Audio::Node
