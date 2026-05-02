#include "Pch.hpp"
#include "Processor.hpp"

// 1. Project Headers
#include "Node/Descriptor.hpp"
#include "Node/Effect.hpp"
#include "Node/Queue.hpp"
#include "Resource/Asset.hpp"

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <cassert>
#include <chrono>
#include <cstdint>
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
        for (std::uint64_t i = 0; i < MaxStaticVoices; ++i)
        {
            m_Voices[i].emplace<Audio::StaticVoiceNode>(nullptr, Node::Effect::Parameters{}, Node::Queue::MaxBufferSize, nullptr);
            m_StaticTicketQueue.emplace(static_cast<Identity::Ticket>(i));
        }

        for (std::uint64_t i = 0; i < MaxStreamVoices; ++i)
        {
            m_Voices[MaxStaticVoices + i].emplace<Audio::StreamVoiceNode>(nullptr, Node::Effect::Parameters{}, Node::Queue::MaxBufferSize, nullptr);
            m_StreamTicketQueue.emplace(static_cast<Identity::Ticket>(MaxStaticVoices + i));
        }

        for (std::uint32_t i = 0; i < MaxVoices; ++i)
        {
            m_Generations[i] = Identity::Generation::Initial;
        }
    }

    auto Processor::Process() -> bool
    {
        for (auto it = m_Issued.begin(); it != m_Issued.end();)
        {
            auto& [tag, tickets] = *it;

            std::erase_if(
                tickets,
                [this](Identity::Ticket ticket)
                {
                    auto& path = m_Voices[static_cast<std::uint64_t>(ticket)];

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
                        // ジェネレーションを進める事で、古いチケットを無効化する
                        const auto index = static_cast<std::uint64_t>(ticket);
                        ++m_Generations[index];

                        // チケットをキューに戻す
                        auto& queue = (index < MaxStaticVoices) ? m_StaticTicketQueue : m_StreamTicketQueue;
                        queue.push(ticket);
                        return true;
                    }
                    return false;
                }
            );

            // もしこのタグに関連付けられたチケットが全て処理されていたら、タグエントリごと削除する
            if (tickets.empty())
            {
                it = m_Issued.erase(it);
            }
            else
            {
                ++it;
            }
        }

        return !m_Issued.empty();

#ifdef _DEBUG
        // const auto log = std::format("[Audio] Processor: Audio.Play.Count={}", m_Issued.size());
        // Audio::Engine::Instance().GetDiagnosticsSink().AddEntry({ Diagnostics::Severity::Verbose, log.data() });
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
        for (std::size_t i = 0; i < MaxStreamVoices; ++i)
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
                m_Voices[MaxStaticVoices + i]
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
            m_Voices[index]
        );

        if (!played)
        {
            return {};
        }
        else
        {
            ticketQueue.pop(); // commit
        }

        // タグ管理
        auto& tag = m_Tags[asset->Metadata.Format];
        ++tag;

#ifdef _DEBUG
        // もしインクリメントの結果が無効値になったら、もう一度進めて 0 にする（あるいは1にする）
        if (static_cast<std::uint64_t>(tag) == static_cast<std::uint64_t>(Identity::Tag::Invalid))
        {
            assert(false && "Identity::Tag has overflowed. Consider increasing the underlying type or handling overflow properly.");
            ++tag; // 0 に戻す(uint64_t なので事実上あり得ないが、安全策として)
        }
#endif

        // チケット発行
        m_Issued[tag].push_back(ticket);

        return { static_cast<Identity::Tag>(tag), static_cast<Identity::Ticket>(ticket), m_Generations[static_cast<std::size_t>(ticket)] };
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
            m_Voices[index]
        );
    }

    auto Processor::Stop() -> void
    {
        auto availableTickets = m_Issued | std::views::values | std::views::join;

        std::ranges::for_each(
            availableTickets,
            [this](Identity::Ticket ticket)
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
                    m_Voices[static_cast<std::uint64_t>(ticket)]
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
            m_Voices[index]
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
            m_Voices[index]
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
