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

    class Passthrough
    {
    public:
        Passthrough();

        auto Update(Context& context) -> bool;

    private:
        auto Sweep(Context& context) -> bool;

    private:
        Node::Entry m_Entry{};

        Frames::Buffer m_Buffer{};

        Device::Signal m_Signal{};
    };

} // namespace N503::Audio::Node
