#pragma once

// 1. Project Headers
#include "Packets/Packet.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <semaphore>

namespace N503::Audio::Command
{

    /// @brief
    struct Envelope final
    {
        /// @brief
        Packets::Packet Packet;

        /// @brief
        std::binary_semaphore* Signal = nullptr;
    };

} // namespace N503::Audio::Command
