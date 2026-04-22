#pragma once

// 1. Project Headers
#include "SourceVoice.hpp"

// 2. Project Dependencies
#include <N503/Audio/Format.hpp>
#include <N503/Memory/Storage/Pool.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <vector>

namespace N503::Audio::Device
{

    class Context;

} // namespace N503::Audio::Device

namespace N503::Audio::Device
{

    class SourceVoicePool final
    {
    public:
        constexpr static std::uint32_t MaxAvailableSourceVoices = 64;

    public:
        explicit SourceVoicePool(Context* context);

        ~SourceVoicePool();

        auto Borrow(const Format& format) -> SourceVoice*;

        auto Return(SourceVoice* voice) -> void;

    private:
        N503::Memory::Storage::Pool<SourceVoice> m_Storage{ MaxAvailableSourceVoices };

        Context* m_Context{ nullptr };

        std::vector<SourceVoice*> m_Availables;

        std::vector<SourceVoice*> m_Indexes;
    };

} // namespace N503::Audio::Device
