#pragma once

// 1. Project Headers
#include "../Device/Signal.hpp"
#include "../Frames/Buffer.hpp"
#include "Context.hpp"
#include "Entry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Node
{

    /// @brief
    class Passthrough
    {
    public:
        /// @brief
        /// @param bytesPerFrame
        Passthrough(const std::size_t bytesPerFrame);

        /// @brief
        /// @param context
        /// @return
        auto Update(Context& context) -> bool;

        /// @brief
        /// @return
        auto Sweep(Context& context) -> bool;

    private:
        /// @brief
        Node::Entry m_Entry{};

        /// @brief
        Frames::Buffer m_Buffer{};

        /// @brief
        Device::Signal m_Signal{};
    };

} // namespace N503::Audio::Node
