#pragma once

#include <N503/Audio/Source.hpp>
#include <N503/Audio/Types.hpp>

namespace N503::Audio
{

    struct Source::Entity final
    {
        Audio::AssetHandle m_AssetHandle{};

        Audio::ProcessHandle m_ProcessHandle{};
    };

} // namespace N503::Audio
