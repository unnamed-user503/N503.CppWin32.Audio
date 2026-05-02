#include "Pch.hpp"
#include "Play.hpp"

// 1. Project Headers
#include "../../System/Processor.hpp"
#include "../Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Message::Packets
{

    auto Play::operator()(const Context& context) const -> void
    {
        if (auto asset = context.ResourceContainer.GetAsset(Handle))
        {
            if (auto handle = context.SystemProcessor.Play(asset))
            {
                *Result = handle;
            }
        }
    }

} // namespace N503::Audio::Message::Packets
