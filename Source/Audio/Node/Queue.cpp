#include "Pch.hpp"
#include "Queue.hpp"

// 1. Project Headers
#include "../Device/Signal.hpp"
#include "../Engine.hpp"
#include "../Frames/Buffer.hpp"
#include "Context.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Audio/Status.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>
#include <cstddef>
#include <format>

namespace N503::Audio::Node
{

    Queue::Queue(const std::size_t bytesPerFrame) : m_Storage{ bytesPerFrame, MaxBuffersQueue }, m_BytesPerFrame{ bytesPerFrame }
    {
        for (std::size_t i = 0; i < MaxBuffersQueue; ++i)
        {
            m_Entries[i].Frames = &m_Buffers[i];
            m_Entries[i].Signal = &m_Signals[i];
            m_Entries[i].Status = Node::Entry::Status::Empty;

            m_Signals[i].Notify.store(false, std::memory_order_relaxed);
            m_Signals[i].Event.store(Device::Signal::Event::None, std::memory_order_relaxed);

            m_Buffers[i].Bytes = nullptr;
            m_Buffers[i].Size = 0;
            m_Buffers[i].Count = 0;
            m_Buffers[i].IsEndOfStream = false;
            m_Buffers[i].Duration = std::chrono::duration<double>(0.0);
        }
    }

    auto Queue::OnPlay() -> void
    {
#ifdef _DEBUG
        Audio::Engine::Instance().GetDiagnosticsSink().AddEntry("[Queue::OnPlay]");
#endif
        for (std::size_t i = 0; i < MaxBuffersQueue; ++i)
        {
            if (m_Buffers[i].Bytes == nullptr)
            {
                auto* address = m_Storage.AllocateBytes(m_BytesPerFrame, static_cast<std::size_t>(16));

                if (!address)
                {
                    break;
                }

                m_Buffers[i].Bytes = static_cast<std::byte*>(address);
                m_Buffers[i].Size = m_BytesPerFrame;
                m_Buffers[i].Count = 0;
                m_Buffers[i].IsEndOfStream = false;
                m_Buffers[i].Duration = std::chrono::duration<double>(0.0);
            }

            m_Entries[i].Frames = &m_Buffers[i];
            m_Entries[i].Signal = &m_Signals[i];
        }
    }

    auto Queue::OnStop() -> void
    {
#ifdef _DEBUG
        Audio::Engine::Instance().GetDiagnosticsSink().AddEntry("[Queue::OnStop]");
#endif
        for (std::size_t i = 0; i < MaxBuffersQueue; ++i)
        {
            m_Entries[i].Frames->Count = 0;
            m_Entries[i].Frames->IsEndOfStream = false;
            m_Entries[i].Frames->Duration = std::chrono::duration<double>(0.0);
            m_Entries[i].Signal->Notify.store(false, std::memory_order_relaxed);
            m_Entries[i].Status = Node::Entry::Status::Empty;
        }
    }

    auto Queue::Update(Context& context) -> bool
    {
        if (context.Descriptor.Status == Audio::Status::Paused)
        {
            return false; // 再生処理を継続する
        }

        if (context.Descriptor.Status == Audio::Status::Stopping)
        {
            return true; // 再生処理を終了する
        }

        if (Sweep(context))
        {
            if (context.Descriptor.Status == Audio::Status::Playing)
            {
                context.Descriptor.Status = Audio::Status::Stopping;
            }

            return false; // 再生処理を継続する (FadeOutによる残務処理を行う為)
        }

        // Contextに書き込み用バッファ(Submit)と読み取り用バッファ(Cache)を渡す
        if (!context.Buffers.Submit && !context.Buffers.Cache)
        {
            context.Buffers.Submit = &m_Entries[0];
            context.Buffers.Cache = &m_Entries[MaxBuffersQueue - 1];
        }

        // 読み取りが終わったバッファ(Status::Present)があればSubmitバッファとして使用する
        std::size_t marked = 0;
        for (std::size_t i = 0; i < MaxBuffersQueue && marked < PrefetchBuffersQueue; ++i)
        {
            if (m_Entries[i].Status == Node::Entry::Status::Present)
            {
                context.Buffers.Submit = &m_Entries[i];
                context.Buffers.Submit->Status = Node::Entry::Status::Pending;
                ++marked;
                break;
            }
        }

        // 空のバッファがあれば読み取り用に準備する
        for (std::size_t i = MaxBuffersQueue; i > 0; --i)
        {
            const std::size_t index = i - 1;
            if (m_Entries[index].Status == Node::Entry::Status::Empty)
            {
                context.Buffers.Cache = &m_Entries[index];
                if (context.Buffers.Cache->Status == Node::Entry::Status::Empty || context.Buffers.Cache->Status == Node::Entry::Status::Completed)
                {
                    context.Buffers.Cache->Status = Node::Entry::Status::Ready;
                    context.Buffers.Cache->Signal->Notify.store(false, std::memory_order_relaxed);

                    context.Buffers.Cache->Frames->Count = 0;
                    context.Buffers.Cache->Frames->IsEndOfStream = false;
                    context.Buffers.Cache->Frames->Duration = std::chrono::duration<double>(0.0);
                }
                break;
            }
        }

        return true; // 再生処理を継続する
    }

    auto Queue::Sweep(Context& context) -> bool
    {
        bool isEndOfStreamReached = false;

        for (std::size_t i = 0; i < MaxBuffersQueue; ++i)
        {
            if (m_Entries[i].Signal->Notify.load(std::memory_order_acquire))
            {
                auto event = m_Entries[i].Signal->Event.load(std::memory_order_relaxed);

                switch (event)
                {
                    case Device::Signal::Event::BufferStart:
                    {
                        if (context.Descriptor.Status == Audio::Status::Loading)
                        {
                            context.Descriptor.Status = Audio::Status::Playing;
                        }

                        if (m_Entries[i].Status == Node::Entry::Status::Submitted)
                        {
                            m_Entries[i].Status = Node::Entry::Status::Playing;
                        }
                    }
                    break;

                    case Device::Signal::Event::BufferEnd:
                    {
                        if (context.Descriptor.Status == Audio::Status::Playing && m_Entries[i].Frames->IsEndOfStream)
                        {
                            context.Descriptor.Status = Audio::Status::Stopping;
                        }

                        // 重要：短いバッファでは BufferStart 通知より先に（あるいは同時に）
                        // Endが届くことがあるため、Submitted からの直接遷移を許可する
                        if (m_Entries[i].Status == Node::Entry::Status::Submitted || m_Entries[i].Status == Node::Entry::Status::Playing)
                        {
                            m_Entries[i].Status = Node::Entry::Status::Completed;
                        }
                    }
                    break;
                }

                // イベント関連のフラグをリセット
                m_Entries[i].Signal->Event.store(Device::Signal::Event::None, std::memory_order_relaxed);
                m_Entries[i].Signal->Notify.store(false, std::memory_order_release);
            }

            if (m_Entries[i].Status == Node::Entry::Status::Completed)
            {
                const bool wasEndOfStream = m_Entries[i].Frames->IsEndOfStream;
#ifdef _DEBUG
                Audio::Engine::Instance().GetDiagnosticsSink().AddEntry(
                    std::format("[Audio] Queue: <Completed> Index={} Bytes={} Size={} Used={} EndOfStream={}", i, static_cast<const void*>(m_Entries[i].Frames->Bytes), m_Entries[i].Frames->Size, m_Entries[i].Frames->Count * m_BytesPerFrame, (wasEndOfStream ? 1 : 0)).data());
#endif
                m_Entries[i].Frames->Count = 0;
                m_Entries[i].Frames->Duration = std::chrono::duration<double>(0.0);
                m_Entries[i].Status = Node::Entry::Status::Empty;

                if (wasEndOfStream)
                {
                    isEndOfStreamReached = true;
                }
            }
        }

        // 全バッファのチェックが終わった後、EOSに達していたら再生終了を通知
        return isEndOfStreamReached;
    }

} // namespace N503::Audio::Node
