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

    class Static
    {
    public:
        explicit Static();

        auto OnPlay(const Audio::Node::Descriptor& descriptor) -> bool;

        ~Static() = default;

        auto Update(Context& context) -> bool;

    private:
        const Resource::Asset* m_Asset{ nullptr };
    };

} // namespace N503::Audio::Node
