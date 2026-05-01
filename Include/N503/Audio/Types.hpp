#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>

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
    }

    struct ProcessHandle final
    {
        Identity::Tag Tag{ Identity::Tag::Invalid };

        Identity::Ticket Ticket{ Identity::Ticket::Invalid };

        Identity::Generation Generation{ Identity::Generation::Initial };

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return Ticket != Identity::Ticket::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return Ticket == Identity::Ticket::Invalid;
        }
    };

    struct AssetHandle final
    {
        Identity::ResourceID ResourceID{ Identity::ResourceID::Invalid };

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ResourceID != Identity::ResourceID::Invalid;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ResourceID == Identity::ResourceID::Invalid;
        }
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

} // namespace N503::Audio
