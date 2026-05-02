#pragma once

// 1. Project Headers
#include "../Device/SourceVoice.hpp"
#include "Context.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Node
{

    class Endpoint
    {
    public:
        explicit Endpoint();

        auto OnPlay(const Audio::Format& format) -> bool;

        auto OnStop() -> void;

        auto Update(Context& context) -> bool;

    private:
        auto Submit(Context& context) -> bool;

    private:
        Device::SourceVoice* m_SourceVoice{ nullptr };
    };

} // namespace N503::Audio::Node
