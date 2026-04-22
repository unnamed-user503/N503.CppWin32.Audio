#pragma once

// 1. Project Headers
#include "../Frames/Buffer.hpp"
#include "../Resource/Metadata.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Resource
{

    struct Asset final
    {
        Audio::AssetHandle Handle;

        Frames::Buffer Frames;

        Resource::Metadata Metadata;
    };

} // namespace N503::Audio::Resource
