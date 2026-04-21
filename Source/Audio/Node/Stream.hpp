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
        /// @brief
        /// @param descriptor
        explicit Stream(const Node::Descriptor *descriptor = nullptr);

        /// @brief
        ~Stream() = default;

        /// @brief
        /// @param descriptor
        /// @return
        auto OnPlay(const Node::Descriptor &descriptor) -> void;

        /// @brief
        /// @return
        auto OnStop() -> void;

        /// @brief
        /// @param context
        /// @return
        auto Update(Context &context) -> bool;

    private:
        /// @brief
        std::unique_ptr<Codec::Decoder> m_Decoder;
    };

} // namespace N503::Audio::Node
