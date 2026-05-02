#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Status.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <type_traits>

namespace N503::Audio
{

    enum class Type : std::uint16_t
    {
        None = 0,
        Static,
        Stream
    };

    namespace Identity
    {
        enum class Ticket : std::uint64_t
        {
            Invalid = std::uint64_t(-1)
        };

        enum class Tag : std::uint64_t
        {
            Invalid = std::uint64_t(-1)
        };

        enum class ResourceID : std::uint64_t
        {
            Invalid = 0
        };

        enum class Generation : std::uint32_t
        {
            Initial = 0
        };

        inline auto operator++(Identity::Tag& tag) noexcept -> Identity::Tag&
        {
            using T = std::underlying_type_t<Identity::Tag>;
            tag     = static_cast<Identity::Tag>(static_cast<T>(tag) + 1);
            return tag;
        }

        inline auto operator++(Identity::Generation& generation) noexcept -> Identity::Generation&
        {
            using T    = std::underlying_type_t<Identity::Generation>;
            generation = static_cast<Identity::Generation>(static_cast<T>(generation) + 1);
            return generation;
        }
    } // namespace Identity

} // namespace N503::Audio
