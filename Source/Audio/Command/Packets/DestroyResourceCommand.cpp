#include "Pch.hpp"
#include "DestroyResourceCommand.hpp"

// 1. Project Headers
#include "../../Engine.hpp"
#include "../../Processor.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Command::Packets
{

    auto DestroyResourceCommand::operator()() const -> void
    {
        Audio::Engine::Instance().GetAudioProcessor().Stop(ProcessHandle);
    }

}
