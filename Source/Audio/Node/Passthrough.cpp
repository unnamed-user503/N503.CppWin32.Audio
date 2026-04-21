#include "Pch.hpp"
#include "Passthrough.hpp"

// 1. Project Headers
#include "../Device/Signal.hpp"
#include "Context.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Audio/Status.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>

namespace N503::Audio::Node
{

    Passthrough::Passthrough(const std::size_t bytesPerFrame)
    {
        m_Entry.Frames = &m_Buffer;
        m_Entry.Signal = &m_Signal;
        m_Entry.Status = Node::Entry::Status::Empty;

        m_Signal.Notify.store(false, std::memory_order_relaxed);
        m_Signal.Event.store(Device::Signal::Event::None, std::memory_order_relaxed);

        m_Buffer.Bytes = nullptr;
        m_Buffer.Size = 0;
        m_Buffer.Count = 0;
        m_Buffer.IsEndOfStream = false;
        m_Buffer.Duration = std::chrono::duration<double>(0.0);
    }

    auto Passthrough::Update(Context &context) -> bool
    {
        if (context.Descriptor.Status == Audio::Status::Paused)
        {
            return false;
        }

        if (context.Descriptor.Status == Audio::Status::Stopping)
        {
            return true; // 再生処理を終了する
        }

        if (!Sweep(context))
        {
            return false;
        }

        // スロットの初期割り当て
        if (!context.Buffers.Submit)
        {
            context.Buffers.Submit = &m_Entry;
        }
        if (!context.Buffers.Cache)
        {
            context.Buffers.Cache = &m_Entry;
        }

        // Submit側の更新: Present なものがあれば Pending にして Endpoint に知らせる
        if (m_Entry.Status == Node::Entry::Status::Present)
        {
            m_Entry.Status = Node::Entry::Status::Pending;
            context.Buffers.Submit = &m_Entry; // 現在送信すべきスロット
        }

        // Cache側の更新: Empty なものがあれば Ready にして Stream に貸し出す
        if (m_Entry.Status == Node::Entry::Status::Empty)
        {
            m_Entry.Frames->Count = 0;
            m_Entry.Frames->IsEndOfStream = false;

            m_Entry.Status = Node::Entry::Status::Ready;
            context.Buffers.Cache = &m_Entry; // 現在受信すべきスロット
        }

        return true;
    }

    auto Passthrough::Sweep(Context &context) -> bool
    {
        if (m_Entry.Signal->Notify.load(std::memory_order_acquire))
        {
            auto event = m_Entry.Signal->Event.load(std::memory_order_relaxed);

            switch (event)
            {
                case Device::Signal::Event::BufferStart:
                {
                    if (context.Descriptor.Status == Audio::Status::Loading)
                    {
                        context.Descriptor.Status = Audio::Status::Playing;
                    }

                    if (m_Entry.Status == Node::Entry::Status::Submitted)
                    {
                        m_Entry.Status = Node::Entry::Status::Playing;
                    }
                }
                break;

                case Device::Signal::Event::BufferEnd:
                {
                    if (context.Descriptor.Status == Audio::Status::Playing && m_Entry.Frames->IsEndOfStream)
                    {
                        context.Descriptor.Status = Audio::Status::Stopping;
                    }

                    if (m_Entry.Status == Node::Entry::Status::Playing)
                    {
                        m_Entry.Status = Node::Entry::Status::Completed;
                    }
                }
                break;
            }

            m_Entry.Signal->Event.store(Device::Signal::Event::None, std::memory_order_relaxed);
            m_Entry.Signal->Notify.store(false, std::memory_order_release);
        }

        // 再生完了したスロットのクリーンアップ
        if (m_Entry.Status == Node::Entry::Status::Completed)
        {
            // 【重要】ここで IsEndOfStream を false に戻すと、Source側での書き込み直後に
            // Sweepが走りフラグを消してしまう事故(アクセス違反)が起きるため、初期化は行わない。
            // m_Entrys[i].Frames->IsEndOfStream = false;

            m_Entry.Frames->Count = 0;
            m_Entry.Frames->Duration = std::chrono::duration<double>(0.0);
            m_Entry.Status = Node::Entry::Status::Empty;

            // 終端フラグが立っているバッファを回収したなら、ストリーム全体の終了とみなす
            if (m_Entry.Frames->IsEndOfStream)
            {
                return false; // 以降の Update 処理を停止させる
            }
            else
            {
                return true; // 次のバッファの処理を続行
            }
        }

        return true;
    }

} // namespace N503::Audio::Node
