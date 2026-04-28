#include "Pch.hpp"
#include "Dispatcher.hpp"

// 1. Project Headers
#include "Context.hpp"
#include "Envelope.hpp"
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Audio::Message
{

    auto Dispatcher::Dispatch(Queue& queue, Context& context) -> void
    {
        auto envelopes = queue.PopAll();

        while (!envelopes.empty())
        {
            auto& bundle = envelopes.front();

            for (auto& packet : bundle.Packets)
            {
                std::visit([&context](auto&& concrete) { concrete(context); }, packet);
            }

            if (bundle.Signal)
            {
                bundle.Signal->release();
            }

            envelopes.pop();
        }
    }

} // namespace N503::Renderer2D::Message
