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

    /// @brief
    class Queue
    {
    public:
        /// @brief
        static constexpr std::size_t MaxBuffersQueue = 4;

        /// @brief
        static constexpr std::size_t MaxBufferSize = 32768;

        /// @brief
        static constexpr std::size_t PrefetchBuffersQueue = 2;

    public:
        /// @brief
        /// @param bytesPerFrame
        explicit Queue(const std::size_t bytesPerFrame);

        /// @brief
        /// @return
        auto OnPlay() -> void;

        /// @brief
        /// @return
        auto OnStop() -> void;

        /// @brief
        /// @param context
        /// @return
        auto Update(Context &context) -> bool;

        /// @brief
        /// @param context
        /// @return
        auto Sweep(Context &context) -> bool;

    private:
        /// @brief
        N503::Memory::Storage::Queue m_Storage;

        /// @brief
        std::array<Node::Entry, MaxBuffersQueue> m_Entries{};

        /// @brief
        std::array<Frames::Buffer, MaxBuffersQueue> m_Buffers{};

        /// @brief
        std::array<Device::Signal, MaxBuffersQueue> m_Signals{};

        /// @brief
        std::size_t m_BytesPerFrame{0};
    };

} // namespace N503::Audio::Node
