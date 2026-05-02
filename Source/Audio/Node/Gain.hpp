#pragma once

// 1. Project Headers
#include "Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Node
{

    class Gain
    {
    public:
        struct Parameters
        {
        };

    public:
        Gain(Parameters parameters);

        auto Update(Context& context) -> bool;

    private:
        Parameters m_Parameters;
    };

} // namespace N503::Audio::Node
