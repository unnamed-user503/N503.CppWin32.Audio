#pragma once

// 1. Project Headers
#include "../../Resource/Handle.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string_view>

// Forward Declarations
namespace N503::Audio::Message
{
    struct Context;
}

namespace N503::Audio::Message::Packets
{

    /// @brief
    struct CreateResource final
    {
        Audio::Resource::Handle* Result;

        Audio::Format Format;

        Audio::Type Type;

        std::string_view Path;

        auto operator()(const Context& context) const -> void;
    };

} // namespace N503::Audio::Command::Packets
