#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include "CreateResourceCommand.hpp"
#include "DestroyResourceCommand.hpp"
#include "PauseCommand.hpp"
#include "PlayCommand.hpp"
#include "ResumeCommand.hpp"
#include "StopCommand.hpp"

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Audio::Command::Packets
{

    using Packet = std::variant<CreateResourceCommand, DestroyResourceCommand, PlayCommand, StopCommand, PauseCommand, ResumeCommand>;

} // namespace N503::Audio::Command::Packets
