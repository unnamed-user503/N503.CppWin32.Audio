#pragma once

// 1. Project Headers
#include "../Codec/Decoder.hpp"
#include "Context.hpp"
#include "Descriptor.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>

namespace N503::Audio::Node
{

    class Stream
    {
    public:
        explicit Stream(const Node::Descriptor* descriptor = nullptr);

        ~Stream() = default;

        auto OnPlay(const Node::Descriptor& descriptor) -> void;

        auto OnStop() -> void;

        auto Update(Context& context) -> bool;

    private:
        std::unique_ptr<Codec::Decoder> m_Decoder;
    };

} // namespace N503::Audio::Node
