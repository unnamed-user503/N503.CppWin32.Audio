#pragma once

// 1. Project Headers
#include <N503/Audio/Details/Api.h>
#include <N503/Audio/Types.hpp>

// 2. Project Dependencies

// 6. C++ Standard Libraries
#include <string_view>
#include <utility> // std::exchange 用

namespace N503::Audio
{
    class Source final
    {
    public:
        explicit Source(std::string_view filepath, Audio::Type audioType = Audio::Type::Stream) : m_Instance(nullptr)
        {
            m_Instance = N503CreateSound(filepath.data(), static_cast<uint32_t>(audioType));
        }

        ~Source()
        {
            if (m_Instance)
            {
                N503DestroySound(m_Instance);
                m_Instance = nullptr;
            }
        }

        Source(const Source&) = delete;

        auto operator=(const Source&) -> Source& = delete;

        Source(Source&& other) noexcept : m_Instance(std::exchange(other.m_Instance, nullptr))
        {
        }

        auto operator=(Source&& other) noexcept -> Source&
        {
            if (this != &other)
            {
                if (m_Instance)
                {
                    N503DestroySound(m_Instance);
                }

                m_Instance = std::exchange(other.m_Instance, nullptr);
            }

            return *this;
        }

    public:
        void Play()
        {
            if (m_Instance)
            {
                N503PlaySound(m_Instance);
            }
        }

        void Stop()
        {
            if (m_Instance)
            {
                N503StopSound(m_Instance);
            }
        }

        void Resume()
        {
            if (m_Instance)
            {
                N503ResumeSound(m_Instance);
            }
        }

        void Pause()
        {
            if (m_Instance)
            {
                N503PauseSound(m_Instance);
            }
        }

    private:
        N503Sound m_Instance;
    };

} // namespace N503::Audio
