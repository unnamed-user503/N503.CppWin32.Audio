#include "Pch.hpp"
#include "Gain.hpp"

// 1. Project Headers
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Status.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <chrono>
#include <cstdlib>

namespace N503::Audio::Node
{

    Gain::Gain()
    {
    }

    auto Gain::Update(Context& context) -> bool
    {
        context.Descriptor.Volume = 1.0f;
        return true;
    }

} // namespace N503::Audio::Node
