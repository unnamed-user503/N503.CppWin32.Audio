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
        explicit Endpoint(Device::SourceVoice* sourceVoice);

        auto OnPlay(const Audio::Format& format) -> void;

        auto OnStop() -> void;

        auto Update(Context& context) -> bool;

    private:
        auto Submit(Context& context) -> bool;

    private:
        Device::SourceVoice* m_SourceVoice;
    };

} // namespace N503::Audio::Node
