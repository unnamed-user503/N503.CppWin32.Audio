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

    /// @brief
    class Effect
    {
    public:
        /// @brief 今後の拡張用
        struct Parameters
        {
        };

    public:
        /// @brief
        /// @param parameters
        Effect(Parameters parameters);

        /// @brief
        /// @param context
        /// @return
        auto Update(Context &context) -> bool;

    private:
        /// @brief
        Parameters m_Parameters;
    };

} // namespace N503::Audio::Node
