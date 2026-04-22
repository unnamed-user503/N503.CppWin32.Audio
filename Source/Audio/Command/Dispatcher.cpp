#include "Pch.hpp"
#include "Dispatcher.hpp"

// 1. Project Headers
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <variant>

namespace N503::Audio::Command
{

    /// @brief
    /// @return
    auto Dispatcher::Dispatch(Command::Queue& queue) -> void
    {
        auto envelopes = queue.PopAll();

        while (!envelopes.empty())
        {
            auto command = envelopes.front();

            std::visit([](auto&& concrete) { concrete(); }, command.Packet);

            if (command.Signal)
            {
                command.Signal->release();
            }

            envelopes.pop();
        }
    }

}; // namespace N503::Audio::Command
