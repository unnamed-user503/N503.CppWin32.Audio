#pragma once

// 1. Project Headers
#include "Packets/CreateResource.hpp"
#include "Packets/DestroyResource.hpp"
#include "Packets/Pause.hpp"
#include "Packets/Play.hpp"
#include "Packets/Resume.hpp"
#include "Packets/Stop.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Audio::Message
{

    using namespace Packets;

    using Packet = std::variant<CreateResource, DestroyResource, Play, Stop, Resume, Pause>;

} // namespace N503::Audio::Message
