#include "Pch.hpp"
#include "Source_Entity.hpp"

#include "Command/Packets/CreateResourceCommand.hpp"
#include "Command/Packets/PauseCommand.hpp"
#include "Command/Packets/PlayCommand.hpp"
#include "Command/Packets/ResumeCommand.hpp"
#include "Command/Packets/StopCommand.hpp"
#include "Command/Queue.hpp"
#include "Engine.hpp"
#include <N503/Audio/Format.hpp>
#include <N503/Audio/Source.hpp>
#include <N503/Audio/Types.hpp>
#include <memory>
#include <string_view>
#include <utility>

namespace N503::Audio
{

    /// @brief
    /// @param filepath
    Source::Source(std::string_view filepath)
    {
        m_Entity = std::make_unique<Source::Entity>();

        auto packet = Command::Packets::CreateResourceCommand{
            .Result = &m_Entity->m_AssetHandle,
            .Format = Audio::Format::Default(),
            .Type = Audio::Type::Stream,
            .Path = filepath.data(),
        };

        Engine::Instance().GetCommandQueue().PushSync(std::move(packet));

        if (!m_Entity->m_AssetHandle)
        {
            m_Entity.reset();
        }
    }

    /// @brief
    Source::~Source()
    {
    }

    /// @brief
    /// @return
    auto Source::Play() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::PlayCommand{ .Result = &m_Entity->m_ProcessHandle, .Handle = m_Entity->m_AssetHandle, .Type = Audio::Type::Stream };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    /// @brief
    /// @return
    auto Source::Stop() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::StopCommand{ .Handle = m_Entity->m_ProcessHandle };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    /// @brief
    /// @return
    auto Source::Pause() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::PauseCommand{ .Handle = m_Entity->m_ProcessHandle };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

    /// @brief
    /// @return
    auto Source::Resume() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Command::Packets::ResumeCommand{ .Handle = m_Entity->m_ProcessHandle };

        Engine::Instance().GetCommandQueue().Push(std::move(packet));
    }

} // namespace N503::Audio
