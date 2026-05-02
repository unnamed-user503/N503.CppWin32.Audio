#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::System
{

    struct Handle final
    {
        Audio::Identity::Tag Tag{ Audio::Identity::Tag::Invalid };

        Audio::Identity::Ticket Ticket{ Audio::Identity::Ticket::Invalid };

        Audio::Identity::Generation Generation{ Audio::Identity::Generation::Initial };

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return Ticket != Audio::Identity::Ticket::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return Ticket == Audio::Identity::Ticket::Invalid;
        }
    };

}
