#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)
#include <wil/com.h>

// 4. Third-party Libraries

// 5. Windows Headers
#include <mfreadwrite.h>

// 6. C++ Standard Libraries

namespace N503::Audio::Codec
{

    class StreamSelector
    {
    public:
        explicit StreamSelector(const wil::com_ptr<IMFSourceReader>& reader);

        auto SelectFirst() -> void;

    private:
        wil::com_ptr<IMFSourceReader> m_SourceReader;
    };

} // namespace N503::Audio::Codec
