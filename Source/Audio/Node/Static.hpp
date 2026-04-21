#pragma once

// 1. Project Headers
#include "../Resource/Asset.hpp"
#include "Context.hpp"
#include "Descriptor.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Node
{

    /// @brief
    class Static
    {
    public:
        /// @brief
        /// @param descriptor
        explicit Static(const Node::Descriptor *descriptor = nullptr);

        /// @brief
        ~Static() = default;

        /// @brief
        /// @param context
        /// @return
        auto Update(Context &context) -> bool;

    private:
        /// @brief
        const Resource::Asset *m_Asset{nullptr};
    };

} // namespace N503::Audio::Node
