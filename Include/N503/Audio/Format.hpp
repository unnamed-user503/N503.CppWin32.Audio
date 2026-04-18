#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <mmreg.h>

// 6. C++ Standard Libraries
#include <compare>
#include <cstdint>
#include <ostream>

namespace N503::Audio
{

// DLL境界を越える際のアライメントを8バイトに固定
#pragma pack(push, 8)

    /// @brief オーディオフォーマット（サンプリングレート、チャンネル数など）を保持する構造体
    struct Format
    {
        /// @brief 1秒あたりのサンプル数 (Hz)
        std::uint32_t SamplePerSecond;

        /// @brief 1秒あたりの平均バイト転送効率
        std::uint32_t BytesPerSecond;

        /// @brief 波形データの形式タグ (例: WAVE_FORMAT_IEEE_FLOAT)
        std::uint16_t Tag;

        /// @brief チャンネル数 (1: モノラル, 2: ステレオ)
        std::uint16_t Channels;

        /// @brief ブロックアライメント (チャンネル数 * サンプルあたりのバイト数)
        std::uint16_t BlockAlign;

        /// @brief 1サンプルあたりのビット数 (例: 16bit, 32bit)
        std::uint16_t BitsPerSample;

        /// @brief 2つのフォーマットが同一であるか比較します
        auto operator<=>(const Format&) const = default;

        /// @brief デフォルトのオーディオ設定（44.1kHz / Stereo / Float）を取得します
        static auto Default() noexcept -> Audio::Format
        {
            return { .SamplePerSecond = 44100,
                     .BytesPerSecond = 44100 * 2 * 4, // 44100 * 8 = 352800
                     .Tag = WAVE_FORMAT_IEEE_FLOAT,
                     .Channels = 2,
                     .BlockAlign = 2 * 4, // 2 * 4 = 8
                     .BitsPerSample = 32 };
        }

        /// @brief Windows API で使用される WAVEFORMATEX 構造体に変換します
        auto ToRawFormat() const noexcept -> WAVEFORMATEX
        {
            return { .wFormatTag = Tag, .nChannels = Channels, .nSamplesPerSec = SamplePerSecond, .nAvgBytesPerSec = BytesPerSecond, .nBlockAlign = BlockAlign, .wBitsPerSample = BitsPerSample, .cbSize = 0 };
        }
    };

#pragma pack(pop)

} // namespace N503::Audio
