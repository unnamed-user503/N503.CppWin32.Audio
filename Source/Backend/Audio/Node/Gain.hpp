#pragma once

// 1. Project Headers
#include "Context.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>

namespace N503::Audio::Node
{

    class Gain
    {
    public:
        Gain();

        auto OnPlay() -> bool;

        auto OnStop() -> void;

        auto Update(Context& context) -> bool;

    private:
        // TODO: Time ノード実装後に移行する
        std::chrono::steady_clock::time_point m_LastTime{};
    };

} // namespace N503::Audio::Node
