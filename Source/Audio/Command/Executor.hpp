#pragma once

// 1. Project Headers
#include "Packets/CreateResourceCommand.hpp"
#include "Packets/DestroyResourceCommand.hpp"
#include "Packets/PauseCommand.hpp"
#include "Packets/PlayCommand.hpp"
#include "Packets/ResumeCommand.hpp"
#include "Packets/StopCommand.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Command
{

    class Executor final
    {
    public:
        Executor() = default;

    public:
        auto operator()(const Packets::CreateResourceCommand&) -> void;

        auto operator()(const Packets::DestroyResourceCommand&) -> void;

        auto operator()(const Packets::PlayCommand&) -> void;

        auto operator()(const Packets::StopCommand&) -> void;

        auto operator()(const Packets::PauseCommand&) -> void;

        auto operator()(const Packets::ResumeCommand&) -> void;

        template <class T> auto operator()(const T& /* command */) -> void { /* nop */ };
    };

} // namespace N503::Audio::Command
