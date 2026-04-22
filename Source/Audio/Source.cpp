#include "Pch.hpp"
#include "Source_Entity.hpp"

// 1. Project Headers
#include "Command/Packets/CreateResourceCommand.hpp"
#include "Command/Packets/DestroyResourceCommand.hpp"
#include "Command/Packets/PauseCommand.hpp"
#include "Command/Packets/PlayCommand.hpp"
#include "Command/Packets/ResumeCommand.hpp"
#include "Command/Packets/StopCommand.hpp"
#include "Command/Queue.hpp"
#include "Engine.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Source.hpp>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <string_view>
#include <utility>

namespace N503::Audio
{

    Source::Source(std::string_view filepath)
    {
        m_Entity = std::make_unique<Source::Entity>();

        auto packet = Command::Packets::CreateResourceCommand{
            .Result = &m_Entity->AssetHandle,
            .Format = Audio::Format::Default(),
            .Type   = Audio::Type::Stream,
            .Path   = filepath.data(),
        };

        Engine::Instance().Start();
        Engine::Instance().GetCommandQueue().PushSync(std::move(packet));

        if (!m_Entity->AssetHandle)
        {
            m_Entity.reset();
        }
    }

    Source::~Source()
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::DestroyResourceCommand{ .Handle = m_Entity->AssetHandle };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    auto Source::Play() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::PlayCommand{ .Result = &m_Entity->ProcessHandle, .Handle = m_Entity->AssetHandle, .Type = Audio::Type::Stream };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    auto Source::Stop() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::StopCommand{ .Handle = m_Entity->ProcessHandle };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    auto Source::Pause() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::PauseCommand{ .Handle = m_Entity->ProcessHandle };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    auto Source::Resume() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::ResumeCommand{ .Handle = m_Entity->ProcessHandle };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

} // namespace N503::Audio
