#pragma once

// 1. Project Headers
#include "Node/Effect.hpp"
#include "Node/Endpoint.hpp"
#include "Node/Passthrough.hpp"
#include "Node/Voice.hpp"
#include "Node/Queue.hpp"
#include "Node/Static.hpp"
#include "Node/Stream.hpp"
#include "Resource/Asset.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <array>
#include <map>
#include <queue>
#include <variant>
#include <vector>

namespace N503::Audio
{

    using StaticVoice = Node::Voice<Node::Static, Node::Effect, Node::Passthrough, Node::Endpoint>;
    using StreamVoice = Node::Voice<Node::Stream, Node::Effect, Node::Queue, Node::Endpoint>;

    using Voice = std::variant<std::monostate, StaticVoice, StreamVoice>;

    class Processor final
    {
        static constexpr std::size_t MaxStaticVoices = 64;

        static constexpr std::size_t MaxStreamVoices = 3;

        static constexpr std::size_t MaxVoices = MaxStaticVoices + MaxStreamVoices;

    public:
        Processor();

        auto Process() -> bool;

        auto Play(const Resource::Asset* asset) -> Audio::ProcessHandle;

        auto Stop(Audio::ProcessHandle handle) -> void;

        auto Pause(Audio::ProcessHandle handle) -> void;

        auto Resume(Audio::ProcessHandle handle) -> void;

        auto Stop() -> void;

        auto WaitForAllStop() -> void;

    private:
        std::array<Audio::Voice, MaxVoices> m_Voices{};

        std::array<Audio::Identity::Generation, MaxVoices> m_Generations{};

        std::queue<Audio::Identity::Ticket> m_StaticTicketQueue{};

        std::queue<Audio::Identity::Ticket> m_StreamTicketQueue{};

        std::map<Audio::Format, Identity::Tag> m_Tags{};

        std::map<Audio::Identity::Tag, std::vector<Audio::Identity::Ticket>> m_Issued{};
    };

} // namespace N503::Audio
