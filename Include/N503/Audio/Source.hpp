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
        /// @brief 指定したファイルからオーディオソースを生成します。
        explicit Source(std::string_view filepath, Audio::Type audioType = Audio::Type::Stream) : m_Handle(nullptr)
        {
            // C-API を呼び出して DLL 内部でインスタンスを生成し、ハンドルを受け取る
            // string_view を C 文字列 (.data()) として渡し、Enum は uint32_t にキャスト
            m_Handle = n503_audio_source_create(filepath.data(), static_cast<uint32_t>(audioType));
        }

        /// @brief リソースを解放します。
        ~Source()
        {
            if (m_Handle)
            {
                n503_audio_source_destroy(m_Handle);
                m_Handle = nullptr;
            }
        }

        // コピー禁止 (リソースの二重解放防止)
        Source(const Source&)                    = delete;
        auto operator=(const Source&) -> Source& = delete;

        // ムーブ許可
        Source(Source&& other) noexcept : m_Handle(std::exchange(other.m_Handle, nullptr))
        {
        }

        auto operator=(Source&& other) noexcept -> Source&
        {
            if (this != &other)
            {
                // 既存のリソースがあれば破棄
                if (m_Handle)
                {
                    n503_audio_source_destroy(m_Handle);
                }
                // ハンドルを奪い取り、相手を空にする
                m_Handle = std::exchange(other.m_Handle, nullptr);
            }
            return *this;
        }

    public:
        void Play()
        {
            if (m_Handle)
            {
                n503_audio_source_play(m_Handle);
            }
        }
        void Stop()
        {
            if (m_Handle)
            {
                n503_audio_source_stop(m_Handle);
            }
        }
        void Pause()
        {
            if (m_Handle)
            {
                n503_audio_source_pause(m_Handle);
            }
        }
        void Resume()
        {
            if (m_Handle)
            {
                n503_audio_source_resume(m_Handle);
            }
        }

    private:
        /// @brief DLL 内部の実体へのハンドル
        n503_audio_source_h m_Handle;
    };

} // namespace N503::Audio
