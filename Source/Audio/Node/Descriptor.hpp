#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Status.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string_view>

namespace N503::Audio::Node
{

    struct Descriptor final
    {
        Audio::AssetHandle Handle{ Audio::Handle::ResourceID::InvalidValue };

        std::string_view Path{};

        Audio::Type Type{ Audio::Type::None };

        Audio::Status Status{ Audio::Status::Stopped };

        float Volume{ 0.0f };

        bool Repeat{ false };
    };

} // namespace N503::Audio::Node
