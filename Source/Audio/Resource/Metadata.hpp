#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Audio::Resource
{

    /// @brief
    struct Metadata final
    {
        Audio::Format Format; ///< オーディオフォーマット

        Audio::Type Type{Audio::Type::None}; ///< Static / Stream

        std::string Path; ///< アセットの実ファイルパス
    };

} // namespace N503::Audio::Resource
