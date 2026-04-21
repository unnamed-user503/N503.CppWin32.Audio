#pragma once

// 1. Project Headers
#include "Queue.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Command
{

    /// @brief
    class Dispatcher final
    {
    public:
        /// @brief
        Dispatcher() = default;

        /// @brief
        ~Dispatcher() = default;

        /// @brief
        /// @param
        Dispatcher(const Dispatcher &) = delete;

        /// @brief
        /// @param
        /// @return
        auto operator=(const Dispatcher &) -> Dispatcher & = delete;

        /// @brief
        /// @param
        Dispatcher(Dispatcher &&) = delete;

        /// @brief
        /// @param
        /// @return
        auto operator=(Dispatcher &&) -> Dispatcher & = delete;

    public:
        /// @brief
        /// @param queue
        /// @param executor
        /// @return
        auto Dispatch(Command::Queue &queue) -> void;
    };

} // namespace N503::Audio::Command
