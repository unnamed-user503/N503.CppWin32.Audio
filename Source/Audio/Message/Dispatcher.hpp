#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Message
{
    struct Context;
    class Queue;
} // namespace N503::Renderer2D::Message

namespace N503::Audio::Message
{

    class Dispatcher final
    {
    public:
        auto Dispatch(Queue& queue, Context& context) -> void;
    };

} // namespace N503::Renderer2D::Message
