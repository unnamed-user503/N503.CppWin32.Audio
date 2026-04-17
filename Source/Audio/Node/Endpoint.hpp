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

    /// @brief
    class Endpoint
    {
    public:
        /// @brief
        /// @param sourceVoice
        explicit Endpoint(Device::SourceVoice* sourceVoice);

        /// @brief
        /// @param format
        /// @return
        auto OnPlay(const Audio::Format& format) -> void;

        /// @brief
        /// @return
        auto OnStop() -> void;

        /// @brief
        /// @param context
        /// @return
        auto Update(Context& context) -> bool;

        /// @brief
        /// @return
        auto Submit() -> bool;

    private:
        /// @brief
        Device::SourceVoice* m_SourceVoice;
    };

} // namespace N503::Audio::Node
