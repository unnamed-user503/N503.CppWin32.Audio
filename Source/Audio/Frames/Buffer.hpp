#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>
#include <cstddef>
#include <cstdint>

namespace N503::Audio::Frames
{

    struct Buffer final
    {
        std::byte* Bytes{ nullptr }; ///< 波形データのアドレス値

        std::size_t Size{ 0 }; ///< 波形データのバイト数

        std::uint64_t Count{ 0 }; ///< フレーム数

        std::chrono::duration<double> Duration{ 0.0 }; ///< 再生時間(ms)

        bool IsEndOfStream{ false }; ///< 最後のフレームバッファ
    };

} // namespace N503::Audio::Frames
