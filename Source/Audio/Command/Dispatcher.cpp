#include "Pch.hpp"
#include "Dispatcher.hpp"

// 1. Project Headers
#include "Executor.hpp"
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
    auto Dispatcher::Dispatch(Queue& queue, Executor& executor) -> void
    {
        auto envelopes = queue.PopAll();

        while (!envelopes.empty())
        {
            auto command = envelopes.front();

            auto delegate = [this, &executor](auto&& concrete)
            {
                executor(concrete);
            };

            std::visit(delegate, command.Packet);

            if (command.Signal)
            {
                command.Signal->release();
            }

            envelopes.pop();
        }
    }

}; // namespace N503::Audio::Command
