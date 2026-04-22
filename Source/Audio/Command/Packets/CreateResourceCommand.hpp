#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string_view>

namespace N503::Audio::Command::Packets
{

    /// @brief
    struct CreateResourceCommand final
    {
        Audio::AssetHandle* Result;

        Audio::Format Format;

        Audio::Type Type;

        std::string_view Path;

        auto operator()() const -> void;
    };

} // namespace N503::Audio::Command::Packets
