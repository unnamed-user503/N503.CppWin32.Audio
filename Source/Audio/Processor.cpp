#include "Pch.hpp"
#include "Processor.hpp"

// 1. Project Headers
#include "Device/SourceVoice.hpp"
#include "Engine.hpp"
#include "Node/Descriptor.hpp"
#include "Node/Effect.hpp"
#include "Node/Queue.hpp"
#include "Resource/Asset.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>
#include <N503/Diagnostics/Severity.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <chrono>
#include <cstdint>
#include <format>
#include <map>
#include <ranges>
#include <thread>
#include <type_traits>
#include <variant>
#include <vector>

namespace N503::Audio
{

    Processor::Processor()
    {
        for (std::uint64_t i = 0; i < MaxStaticVoicePaths; ++i)
        {
            m_VoicePaths[i].emplace<Node::StaticVoicePath>(nullptr, Node::Effect::Parameters{}, Node::Queue::MaxBufferSize, nullptr);
            m_StaticTicketQueue.emplace(static_cast<Audio::Handle::Ticket>(i));
        }

        for (std::uint64_t i = 0; i < MaxStreamVoicePaths; ++i)
        {
            m_VoicePaths[MaxStaticVoicePaths + i].emplace<Node::StreamVoicePath>(nullptr, Node::Effect::Parameters{}, Node::Queue::MaxBufferSize, nullptr);
            m_StreamTicketQueue.emplace(static_cast<Audio::Handle::Ticket>(MaxStaticVoicePaths + i));
        }

        for (std::uint32_t i = 0; i < MaxVoicePaths; ++i)
        {
            m_Generations[i] = Audio::Handle::Generation::InitialValue;
        }
    }

    auto Processor::Process() -> bool
    {
        for (auto& tickets : m_Issued | std::views::values)
        {
            std::erase_if(
                tickets,
                [this](Audio::Handle::Ticket ticket)
                {
                    auto& path = m_VoicePaths[static_cast<std::uint64_t>(ticket)];

                    const bool isFinished = std::visit(
                        [](auto& node) -> bool
                        {
                            using T = std::decay_t<decltype(node)>;
                            if constexpr (std::is_same_v<T, std::monostate>)
                            {
                                return true;
                            }
                            else
                            {
                                return node.Process();
                            }
                        },
                        path
                    );

                    if (isFinished)
                    {
                        auto& queue = (static_cast<std::uint64_t>(ticket) < MaxStaticVoicePaths) ? m_StaticTicketQueue : m_StreamTicketQueue;
                        queue.push(ticket);
                        return true;
                    }

                    return false;
                }
            );
        }

        std::erase_if(m_Issued, [](const auto& pair) { return pair.second.empty(); });

#ifdef _DEBUG
        //const auto log = std::format("[Audio] Processor: Audio.Play.Count={}", m_Issued.size());
        //Audio::Engine::Instance().GetDiagnosticsSink().AddEntry({ Diagnostics::Severity::Verbose, log.data() });
#endif
        return !m_Issued.empty();
    }

    auto Processor::Play(const Resource::Asset* asset) -> Audio::ProcessHandle
    {
        if (!asset)
        {
            return {};
        }

        // Stream再生のみ重複再生を許可しない
        for (std::size_t i = 0; i < MaxStreamVoicePaths; ++i)
        {
            bool found = std::visit(
                [&](auto& node) -> bool
                {
                    using T = std::decay_t<decltype(node)>;
                    if constexpr (std::is_same_v<T, std::monostate>)
                    {
                        return false;
                    }
                    else
                    {
                        return (node.GetAssetHandle().ResourceID == asset->Handle.ResourceID);
                    }
                },
                m_VoicePaths[MaxStaticVoicePaths + i]
            );

            if (found)
            {
                return {};
            }
        }

        // チケットキューの選択
        auto& ticketQueue = (asset->Metadata.Type == Audio::Type::Static) ? m_StaticTicketQueue : m_StreamTicketQueue;

        if (ticketQueue.empty())
        {
            return {};
        }

        // チケットを取得できた場合のみ再生処理を行う
        const auto ticket = ticketQueue.front();
        const auto index  = static_cast<std::uint64_t>(ticket);

        bool played = std::visit(
            [&](auto& node) -> bool
            {
                using T = std::decay_t<decltype(node)>;
                if constexpr (std::is_same_v<T, std::monostate>)
                {
                    return false;
                }
                else
                {
                    Node::Descriptor descriptor{
                        .Handle = asset->Handle,
                        .Path   = asset->Metadata.Path,
                        .Type   = asset->Metadata.Type,
                        .Volume = 1.0f,
                        .Repeat = asset->Metadata.Type == Audio::Type::Stream ? true : false,
                    };

                    return node.Play(descriptor, asset->Metadata.Format);
                }
            },
            m_VoicePaths[index]
        );

        if (!played)
        {
            return {};
        }
        else
        {
            ticketQueue.pop(); // commit
        }

        auto& tag = m_Tags[asset->Metadata.Format];
        ++tag;
        m_Issued[tag].push_back(ticket);

        ++m_Generations[index];

        return { static_cast<Audio::Handle::Tag>(tag), static_cast<Audio::Handle::Ticket>(ticket), m_Generations[static_cast<std::size_t>(ticket)] };
    }

    auto Processor::Stop(Audio::ProcessHandle handle) -> void
    {
        if (!handle)
        {
            return;
        }

        auto index = static_cast<std::size_t>(handle.Ticket);

        if (m_Generations[index] != handle.Generation)
        {
            return;
        }

        std::visit(
            [](auto& node)
            {
                using T = std::decay_t<decltype(node)>;
                if constexpr (!std::is_same_v<T, std::monostate>)
                {
                    node.Stop();
                }
            },
            m_VoicePaths[index]
        );
    }

    auto Processor::Stop() -> void
    {
        auto availableTickets = m_Issued | std::views::values | std::views::join;

        std::ranges::for_each(
            availableTickets,
            [this](Audio::Handle::Ticket ticket)
            {
                std::visit(
                    [](auto& node)
                    {
                        using T = std::decay_t<decltype(node)>;
                        if constexpr (!std::is_same_v<T, std::monostate>)
                        {
                            node.Stop();
                        }
                    },
                    m_VoicePaths[static_cast<std::uint64_t>(ticket)]
                );
            }
        );
    }

    auto Processor::Pause(Audio::ProcessHandle handle) -> void
    {
        if (!handle)
        {
            return;
        }

        auto index = static_cast<std::size_t>(handle.Ticket);

        if (m_Generations[index] != handle.Generation)
        {
            return;
        }

        std::visit(
            [](auto& node)
            {
                using T = std::decay_t<decltype(node)>;
                if constexpr (!std::is_same_v<T, std::monostate>)
                {
                    node.Pause();
                }
            },
            m_VoicePaths[index]
        );
    }

    auto Processor::Resume(Audio::ProcessHandle handle) -> void
    {
        if (!handle)
        {
            return;
        }

        auto index = static_cast<std::size_t>(handle.Ticket);

        if (m_Generations[index] != handle.Generation)
        {
            return;
        }

        std::visit(
            [](auto& node)
            {
                using T = std::decay_t<decltype(node)>;
                if constexpr (!std::is_same_v<T, std::monostate>)
                {
                    node.Resume();
                }
            },
            m_VoicePaths[index]
        );
    }

    auto Processor::WaitForAllStop() -> void
    {
        Stop();

        while (Process())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }

} // namespace N503::Audio
