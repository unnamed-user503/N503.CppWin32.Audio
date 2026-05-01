#include "Pch.hpp"
#include "Stream.hpp"

// 1. Project Headers
#include "../Codec/Decoder.hpp"
#include "../Codec/MediaFoundationDecoder.hpp"
#include "../Engine.hpp"
#include "../Frames/Buffer.hpp"
#include "Context.hpp"
#include "Descriptor.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Audio/Status.hpp>
#include <N503/Audio/Types.hpp>
#include <N503/Diagnostics/Reporter.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <span>
#include <string_view>

namespace N503::Audio::Node
{

    Stream::Stream(const Node::Descriptor* descriptor)
    {
    }

    auto Stream::OnPlay(const Node::Descriptor& descriptor) -> void
    {
#ifdef _DEBUG
        Audio::Engine::GetInstance().GetDiagnosticsReporter().Verbose("[Audio] Stream: OnPlay called.");
#endif
        m_Decoder = std::make_unique<Codec::MediaFoundationDecoder>(descriptor.Path);
    }

    auto Stream::OnStop() -> void
    {
#ifdef _DEBUG
        Audio::Engine::GetInstance().GetDiagnosticsReporter().Verbose("[Audio] Stream: OnStop called.");
#endif
        m_Decoder.reset();
    }

    auto Stream::OnRepeat() -> void
    {
        if (m_Decoder)
        {
            m_Decoder->Seek(0);
        }
    }

    auto Stream::Update(Context& context) -> bool
    {
        if (context.Descriptor.Status == Audio::Status::Stopping)
        {
            return true; // 再生処理を終了する
        }

        if (context.Descriptor.Status == Audio::Status::Paused)
        {
            return false; // 再生処理を継続する
        }

        if (!m_Decoder)
        {
            if (context.Descriptor.Type != Audio::Type::Stream)
            {
                context.Descriptor.Status = Audio::Status::Error;
                return true; // 再生処理を終了する
            }

            if (context.Descriptor.Path.empty())
            {
                context.Descriptor.Status = Audio::Status::Error;
                return true; // 再生処理を終了する
            }

            try
            {
                m_Decoder = std::make_unique<Codec::MediaFoundationDecoder>(context.Descriptor.Path);
            }
            catch (...)
            {
                context.Descriptor.Status = Audio::Status::Error;
                return false; // 再生処理を終了する
            }
        }

        if (!context.Buffers.Cache || context.Buffers.Cache->Status != Node::Entry::Status::Ready)
        {
            return false; // 再生処理を継続する
        }

        const auto format = m_Decoder->GetFormat();
        if (format.BlockAlign == 0)
        {
            context.Descriptor.Status = Audio::Status::Error;
            return true; // 再生処理を終了する
        }

        const std::uint32_t requestedFrames = static_cast<std::uint32_t>(context.Buffers.Cache->Frames->Bytes.size() / format.BlockAlign);

        if (requestedFrames == 0)
        {
            return false; // 再生処理を継続する
        }

        auto result = m_Decoder->Decode(requestedFrames, [&](std::size_t size) { return context.Buffers.Cache->Frames->Bytes; });

        context.Buffers.Cache->Frames->Count         = result.Count;
        context.Buffers.Cache->Frames->IsEndOfStream = result.IsEndOfStream;

        if (0 < result.Count)
        {
            context.Buffers.Cache->Status = Node::Entry::Status::Present;
        }
        else if (result.IsEndOfStream)
        {
            // 1枚も書けなかったが終端に達した場合は、空のまま終端フラグを立てて遷移
            // XAudio2は0バイトのフレームを無視するのでイベントを受信する事が出来ません。
            // そこでここでは空のフレームを送信する事で対応します。
            std::fill_n(context.Buffers.Cache->Frames->Bytes.data(), 4, std::byte{ 0 });
            context.Buffers.Cache->Frames->Count         = 1;
            context.Buffers.Cache->Frames->IsEndOfStream = true;
            context.Buffers.Cache->Status                = Node::Entry::Status::Present;
            return true; // 再生処理を終了する
        }

        return false; // 再生処理を継続する
    }

} // namespace N503::Audio::Node
