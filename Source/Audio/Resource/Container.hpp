#pragma once

// 1. Project Headers
#include "Asset.hpp"
#include "Handle.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>
#include <N503/Memory/Storage/Arena.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <string_view>
#include <vector>

namespace N503::Audio::Resource
{

    class Container final
    {
    public:
        static constexpr std::size_t MaxAssets = 1024;

        explicit Container(std::size_t initialArenaSize = 1024 * 1024 * 16);

        ~Container();

        Container(const Container&) = delete;

        auto operator=(const Container&) -> Container& = delete;

        auto Store(Audio::Type type, std::string_view path) -> Audio::Resource::Handle;

        auto GetAsset(Audio::Resource::Handle handle) const noexcept -> const Resource::Asset*;

        auto Remove(Audio::Resource::Handle handle) -> void;

        auto Reset() -> void;

    private:
        N503::Memory::Storage::Arena m_Storage;

        std::array<Resource::Asset*, MaxAssets + 1> m_AssetSlots{};

        std::vector<Audio::Resource::Handle> m_AvailableHandles;
    };

} // namespace N503::Audio::Resource
