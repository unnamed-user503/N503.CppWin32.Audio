#include "Pch.hpp"

// 1. Project Headers
#include "../../Backend/Audio/Engine.hpp"
#include "../../Backend/Audio/Message/Packets/CreateResource.hpp"
#include "../../Backend/Audio/Message/Packets/DestroyResource.hpp"
#include "../../Backend/Audio/Message/Packets/Pause.hpp"
#include "../../Backend/Audio/Message/Packets/Play.hpp"
#include "../../Backend/Audio/Message/Packets/Resume.hpp"
#include "../../Backend/Audio/Message/Packets/Stop.hpp"
#include "../../Backend/Audio/Message/Queue.hpp"
#include "SourceEntity.hpp"

// 2. Project Dependencies
#include <N503/Audio/Details/Api.h>
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <string_view>
#include <utility>

extern "C"
{
    N503Sound N503CreateSound(const char* path, uint32_t type)
    {
        using namespace N503::Audio;

        try
        {
            auto entity = std::make_unique<SourceEntity>();

            auto packet = Message::Packets::CreateResource{
                .Result = &entity->AssetHandle,
                .Format = Format::Default(),
                .Type   = static_cast<Type>(type),
                .Path   = path,
            };

            Engine::GetInstance().Start();
            Engine::GetInstance().GetMessageQueue().EnqueueSync(std::move(packet));

            if (!entity->AssetHandle)
            {
                throw std::runtime_error("audio creation failed.");
            }

            return reinterpret_cast<N503Sound>(entity.release());
        }
        CATCH_LOG();

        return nullptr;
    }

    int N503DestroySound(N503Sound instance)
    {
        using namespace N503::Audio;

        try
        {
            auto* entity = reinterpret_cast<SourceEntity*>(instance);

            if (!entity)
            {
                throw std::runtime_error("invalid argument.");
            }

            auto packet = Message::Packets::DestroyResource{
                .Handle = entity->AssetHandle,
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
        }
        CATCH_LOG();

        return -1;
    }

    int N503PlaySound(N503Sound instance)
    {
        using namespace N503::Audio;

        try
        {
            auto* entity = reinterpret_cast<SourceEntity*>(instance);

            if (!entity)
            {
                throw std::runtime_error("invalid argument.");
            }

            auto packet = Message::Packets::Play{
                .Result = &entity->ProcessHandle,
                .Handle = entity->AssetHandle,
                .Type   = Type::Stream,
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
        }
        CATCH_LOG();

        return -1;
    }

    int N503StopSound(N503Sound instance)
    {
        using namespace N503::Audio;

        try
        {
            auto* entity = reinterpret_cast<SourceEntity*>(instance);

            if (!entity)
            {
                throw std::runtime_error("invalid argument.");
            }

            auto packet = Message::Packets::Stop{
                .Handle = entity->ProcessHandle,
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
        }
        CATCH_LOG();

        return -1;
    }

    int N503ResumeSound(N503Sound instance)
    {
        using namespace N503::Audio;

        try
        {
            auto* entity = reinterpret_cast<SourceEntity*>(instance);

            if (!entity)
            {
                throw std::runtime_error("invalid argument.");
            }

            auto packet = Message::Packets::Resume{
                .Handle = entity->ProcessHandle,
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
        }
        CATCH_LOG();

        return -1;
    }

    int N503PauseSound(N503Sound instance)
    {
        using namespace N503::Audio;

        try
        {
            auto* entity = reinterpret_cast<SourceEntity*>(instance);

            if (!entity)
            {
                throw std::runtime_error("invalid argument.");
            }

            auto packet = Message::Packets::Pause{
                .Handle = entity->ProcessHandle,
            };

            Engine::GetInstance().GetMessageQueue().Enqueue(std::move(packet));
        }
        CATCH_LOG();

        return -1;
    }
}
