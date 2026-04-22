#include "Pch.hpp"
#include "Container.hpp"

// 1. Project Headers
#include "../Codec/MediaFoundationDecoder.hpp"
#include "Asset.hpp"
#include "Metadata.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iterator>
#include <memory>
#include <ranges>
#include <span>
#include <string>
#include <string_view>

namespace N503::Audio::Resource
{

    Container::Container(std::size_t initialArenaSize) : m_Storage(initialArenaSize)
    {
        Reset();
    }

    Container::~Container()
    {
    }

    auto Container::Store(Audio::Type type, std::string_view path) -> Audio::AssetHandle
    {
        if (path.empty())
        {
            return { Audio::Handle::ResourceID::InvalidValue };
        }

        Audio::AssetHandle handle = m_AvailableHandles.back();
        m_AvailableHandles.pop_back();

        Codec::MediaFoundationDecoder decoder(path);

        auto* address = m_Storage.AllocateBytes(sizeof(Resource::Asset), alignof(Resource::Asset));
        auto* asset   = std::construct_at(reinterpret_cast<Resource::Asset*>(address));

        asset->Handle          = handle;
        asset->Frames.Count    = decoder.GetTotalFrames();
        asset->Frames.Duration = std::chrono::duration<double>{ decoder.GetTotalDurations() };
        asset->Metadata.Format = decoder.GetFormat();
        asset->Metadata.Path   = std::string(path);
        asset->Metadata.Type   = type;

        if (asset->Metadata.Type == Audio::Type::Static)
        {
            const std::size_t totalBytes = asset->Frames.Count * asset->Metadata.Format.BlockAlign;
            auto* persistent             = m_Storage.AllocateBytes(totalBytes, 16);

            decoder.Decode(
                static_cast<std::uint32_t>(asset->Frames.Count),
                [&](std::size_t size) { return std::span<std::byte>(static_cast<std::byte*>(persistent), size); }
            );

            asset->Frames.Bytes         = static_cast<std::byte*>(persistent);
            asset->Frames.Size          = totalBytes;
            asset->Frames.Count         = static_cast<std::uint32_t>(asset->Frames.Count);
            asset->Frames.IsEndOfStream = true;
        }

        m_AssetSlots[static_cast<std::size_t>(handle.ResourceID)] = asset;

        return handle;
    }

    auto Container::GetAsset(Audio::AssetHandle handle) const noexcept -> const Resource::Asset*
    {
        auto index = static_cast<std::size_t>(handle.ResourceID);

        if (index == 0 || index > MaxAssets)
        {
            return nullptr;
        }

        return m_AssetSlots[index];
    }

    auto Container::Remove(Audio::AssetHandle handle) -> void
    {
        auto index = static_cast<std::size_t>(handle.ResourceID);

        if (index == 0 || index > MaxAssets)
        {
            return;
        }

        if (m_AssetSlots[index])
        {
            m_AssetSlots[index] = nullptr;
            m_AvailableHandles.push_back(handle);
        }
    }

    auto Container::Reset() -> void
    {
        m_Storage.Reset();
        m_AssetSlots.fill(nullptr);

        m_AvailableHandles.clear();
        m_AvailableHandles.reserve(MaxAssets);

        auto resourceIds = std::views::iota(0ULL, static_cast<unsigned long long>(MaxAssets));

        // clang-format off
        auto handleView = resourceIds | std::views::transform([](auto i)
        {
            return Audio::AssetHandle{ .ResourceID = static_cast<Audio::Handle::ResourceID>(i) };
        });
        // clang-format on

        std::ranges::copy(handleView, std::back_inserter(m_AvailableHandles));
    }

} // namespace N503::Audio::Resource
