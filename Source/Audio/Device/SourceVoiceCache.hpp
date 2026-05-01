#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Audio/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <cstdint>
#include <vector>

namespace N503::Audio::Device
{
    class MasterVoice;
    class SourceVoice;
} // namespace N503::Audio::Device

namespace N503::Audio::Device
{

    class SourceVoiceCache final
    {
    public:
        constexpr static std::uint32_t MaxAvailableSourceVoices = 64;

    public:
        explicit SourceVoiceCache(MasterVoice* masterVoice);

        ~SourceVoiceCache();

        auto Borrow(const Format& format) -> SourceVoice*;

        auto Return(SourceVoice* voice) -> void;

    private:
        MasterVoice* m_MasterVoice{ nullptr };

        std::vector<std::unique_ptr<SourceVoice>> m_Storage;

        std::vector<SourceVoice*> m_Availables;
    };

} // namespace N503::Audio::Device
