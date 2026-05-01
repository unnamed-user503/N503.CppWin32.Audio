#include "Pch.hpp"
#include "Static.hpp"

// 1. Project Headers
#include "../Engine.hpp"
#include "../Frames/Buffer.hpp"
#include "Context.hpp"
#include "Descriptor.hpp"
#include "Entry.hpp"

// 2. Project Dependencies
#include <N503/Audio/Status.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Audio::Node
{

    Static::Static(const Node::Descriptor* descriptor)
    {
    }

    auto Static::Update(Context& context) -> bool
    {
        if (context.Descriptor.Status == Audio::Status::Stopping)
        {
            return true; // 再生処理を終了する
        }

        if (context.Descriptor.Status == Audio::Status::Paused)
        {
            return false; // 再異性処理を継続する
        }

        if (!m_Asset)
        {
            m_Asset = Audio::Engine::GetInstance().GetResourceContainer().GetAsset(context.Descriptor.Handle);

            if (!m_Asset)
            {
#ifdef _DEBUG
                Audio::Engine::GetInstance().GetDiagnosticsReporter().Error("[Audio] Node::Static: invalid audio handle.");
#endif
                return true; // 再生処理を終了する
            }
        }

        if (!context.Buffers.Cache || context.Buffers.Cache->Status != Node::Entry::Status::Ready)
        {
            return false; // 再生処理を継続する
        }

        if (m_Asset->Frames.Bytes.empty())
        {
#ifdef _DEBUG
            Audio::Engine::GetInstance().GetDiagnosticsReporter().Error("[Audio] Node::Static: invalid audio frames.");
#endif
            return true; // 再生処理を終了する
        }

        *context.Buffers.Cache->Frames               = m_Asset->Frames;
        context.Buffers.Cache->Frames->IsEndOfStream = true;
        context.Buffers.Cache->Status                = Node::Entry::Status::Present;

        return false; // 再生処理を継続する
    }

} // namespace N503::Audio::Node
