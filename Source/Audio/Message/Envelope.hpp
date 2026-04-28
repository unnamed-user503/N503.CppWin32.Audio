#pragma once

// 1. Project Headers
#include "Packet.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <semaphore>
#include <vector>

namespace N503::Audio::Message
{

    struct Envelope
    {
        std::vector<Packet> Packets;

        std::binary_semaphore* Signal{ nullptr };

        Envelope() = default;

        Envelope(Packet&& packet) : Signal(nullptr)
        {
            Packets.reserve(1);
            Packets.push_back(std::move(packet));
        }

        Envelope(Packet&& packet, std::binary_semaphore* signal) : Signal(signal)
        {
            Packets.reserve(1);
            Packets.push_back(std::move(packet));
        }

        Envelope(std::vector<Packet>&& packets, std::binary_semaphore* signal) : Packets(std::move(packets)), Signal(signal)
        {
        }
    };

} // namespace N503::Renderer2D::Message
