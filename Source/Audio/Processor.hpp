#pragma once

// 1. Project Headers
#include "Node/Effect.hpp"
#include "Node/Endpoint.hpp"
#include "Node/Passthrough.hpp"
#include "Node/Path.hpp"
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

namespace N503::Audio::Node
{

    using StaticVoicePath = Node::Path<Node::Static, Node::Effect, Node::Passthrough, Node::Endpoint>;
    using StreamVoicePath = Node::Path<Node::Stream, Node::Effect, Node::Queue, Node::Endpoint>;

    using VoicePath = std::variant<std::monostate, StaticVoicePath, StreamVoicePath>;

} // namespace N503::Audio::Node

namespace N503::Audio
{

    class Processor final
    {
        static constexpr std::size_t MaxStaticVoicePaths = 64;

        static constexpr std::size_t MaxStreamVoicePaths = 3;

        static constexpr std::size_t MaxVoicePaths = MaxStaticVoicePaths + MaxStreamVoicePaths;

    public:
        Processor();

        auto Process() -> bool;

        auto Play(const Resource::Asset *asset) -> Audio::ProcessHandle;

        auto Stop(Audio::ProcessHandle handle) -> void;

        auto Pause(Audio::ProcessHandle handle) -> void;

        auto Resume(Audio::ProcessHandle handle) -> void;

        auto Stop() -> void;

        auto WaitForAllStop() -> void;

    private:
        std::array<Node::VoicePath, MaxVoicePaths> m_VoicePaths{};

        std::array<Handle::Generation, MaxVoicePaths> m_Generations{};

        std::queue<Audio::Handle::Ticket> m_StaticTicketQueue{};

        std::queue<Audio::Handle::Ticket> m_StreamTicketQueue{};

        std::map<Audio::Format, Audio::Handle::Tag> m_Tags{};

        std::map<Audio::Handle::Tag, std::vector<Audio::Handle::Ticket>> m_Issued{};
    };

} // namespace N503::Audio
