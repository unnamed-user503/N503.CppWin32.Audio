#pragma once

// 1. Project Headers
#include "Resource/Handle.hpp"
#include "System/Handle.hpp"

// 2. Project Dependencies
#include <N503/Audio/Source.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio
{

    struct Source::Entity final
    {
        Audio::Resource::Handle AssetHandle{};

        Audio::System::Handle ProcessHandle{};
    };

} // namespace N503::Audio
