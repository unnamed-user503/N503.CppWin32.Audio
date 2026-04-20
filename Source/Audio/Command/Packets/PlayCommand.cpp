#include "Pch.hpp"
#include "PlayCommand.hpp"

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

    auto PlayCommand::operator()() const -> void
    {
        if (auto asset = Audio::Engine::Instance().GetResourceContainer().GetAsset(Handle))
        {
            *Result = Audio::Engine::Instance().GetAudioProcessor().Play(asset);
        }
    }

}
