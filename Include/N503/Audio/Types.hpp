#pragma once

// 1. Project Headers

// 2. Project Dependencies

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

    namespace Handle
    {
        enum class Ticket : std::uint64_t
        {
            InvalidValue = std::uint64_t(-1)
        };

        enum class Tag : std::uint64_t
        {
            InvalidValue = std::uint64_t(-1)
        };

        enum class ResourceID : std::uint64_t
        {
            InvalidValue = 0
        };

        enum class Generation : std::uint32_t
        {
            InitialValue = 0
        };
    } // namespace Handle

    struct ProcessHandle final
    {
        Handle::Tag Tag{ Handle::Tag::InvalidValue };

        Handle::Ticket Ticket{ Handle::Ticket::InvalidValue };

        Handle::Generation Generation{ Handle::Generation::InitialValue };

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return Ticket != Handle::Ticket::InvalidValue;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return Ticket == Handle::Ticket::InvalidValue;
        }
    };

    struct AssetHandle final
    {
        Handle::ResourceID ResourceID;

        [[nodiscard]]
        explicit operator bool() const noexcept
        {
            return ResourceID != Handle::ResourceID::InvalidValue;
        }

        [[nodiscard]]
        bool operator!() const noexcept
        {
            return ResourceID == Handle::ResourceID::InvalidValue;
        }
    };

    inline auto operator++(Audio::Handle::Tag& tag) noexcept -> Audio::Handle::Tag&
    {
        using T = std::underlying_type_t<Audio::Handle::Tag>;
        tag = static_cast<Audio::Handle::Tag>(static_cast<T>(tag) + 1);
        return tag;
    }

    inline auto operator++(Audio::Handle::Generation& generation) noexcept -> Audio::Handle::Generation&
    {
        using T = std::underlying_type_t<Audio::Handle::Generation>;
        generation = static_cast<Audio::Handle::Generation>(static_cast<T>(generation) + 1);
        return generation;
    }

} // namespace N503::Audio
