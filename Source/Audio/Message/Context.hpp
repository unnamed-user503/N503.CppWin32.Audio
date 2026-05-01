#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

// Forward Declarations
namespace N503::Audio::Resource
{
    class Container;
}

// Declarations
namespace N503::Audio::Message
{

    struct Context
    {
        Audio::Resource::Container& ResourceContainer;
    };

}
