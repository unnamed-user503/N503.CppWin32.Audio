#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Resource
{

    struct Handle final
    {
        Identity::ResourceID ResourceID{ Identity::ResourceID::Invalid };

        Identity::Generation Generation{ Identity::Generation::Initial };

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ResourceID != Identity::ResourceID::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ResourceID == Identity::ResourceID::Invalid;
        }
    };

} // namespace N503::Audio::Resource
