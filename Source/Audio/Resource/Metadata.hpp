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

    struct Metadata final
    {
        Audio::Format Format;

        Audio::Type Type{ Audio::Type::None };

        std::string Path;
    };

} // namespace N503::Audio::Resource
