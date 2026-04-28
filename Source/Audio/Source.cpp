#include "Pch.hpp"
#include "Source_Entity.hpp"

// 1. Project Headers
#include "Message/Packets/CreateResource.hpp"
#include "Message/Packets/DestroyResource.hpp"
#include "Message/Packets/Pause.hpp"
#include "Message/Packets/Play.hpp"
#include "Message/Packets/Resume.hpp"
#include "Message/Packets/Stop.hpp"
#include "Message/Queue.hpp"
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

        auto packet = Message::Packets::CreateResource{
            .Result = &m_Entity->AssetHandle,
            .Format = Audio::Format::Default(),
            .Type   = Audio::Type::Stream,
            .Path   = filepath.data(),
        };

        Engine::GetInstance().Start();
        Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));

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

        auto packet = Message::Packets::DestroyResource{ .Handle = m_Entity->AssetHandle };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Source::Play() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::Play{ .Result = &m_Entity->ProcessHandle, .Handle = m_Entity->AssetHandle, .Type = Audio::Type::Stream };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Source::Stop() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::Stop{ .Handle = m_Entity->ProcessHandle };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Source::Pause() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::Pause{ .Handle = m_Entity->ProcessHandle };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

    auto Source::Resume() -> void
    {
        if (!m_Entity)
        {
            return;
        }

        auto packet = Message::Packets::Resume{ .Handle = m_Entity->ProcessHandle };

        Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
    }

} // namespace N503::Audio
