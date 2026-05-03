#pragma once

// 1. Project Headers
#include "../../Backend/Audio/Resource/Handle.hpp"
#include "../../Backend/Audio/System/Handle.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio
{

    struct SourceEntity final
    {
        Audio::Resource::Handle AssetHandle{};

        Audio::System::Handle ProcessHandle{};
    };

} // namespace N503::Audio
