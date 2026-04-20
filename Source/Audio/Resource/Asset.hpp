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

    /// @brief
    struct Asset final
    {
        Audio::AssetHandle Handle; ///< アセット管理用ID

        Frames::Buffer Frames; ///< 波形データ

        Resource::Metadata Metadata; ///< 再生に必要な情報
    };

} // namespace N503::Audio::Resource
