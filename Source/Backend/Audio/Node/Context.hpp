#pragma once

// 1. Project Headers
#include "Descriptor.hpp"
#include "Entry.hpp"

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <chrono>
#include <cstdint>

namespace N503::Audio::Node
{

    // clang-format off
    struct Context final
    {
        Node::Descriptor Descriptor;

        /// @brief 
        struct
        {
            Node::Entry* Submit{ nullptr };   
            Node::Entry* Cache{ nullptr };    
        }
        Buffers;

        struct
        {
            struct
            {
                std::chrono::microseconds Elapsed{ 0 };    
                std::chrono::microseconds Direction{ 0 };  
                std::chrono::microseconds Threshold{ 0 };  
            }
            Fade;
        }
        Effect;

        struct
        {
            std::uint64_t TotalSubmittedCount{ 0 };    
            std::uint64_t TotalCompletedCount{ 0 };    
            std::uint64_t TotalSubmittedFrame{ 0 };    
            std::uint64_t TotalCompletedFrame{ 0 };    
        }
        Statistics;

        struct
        {
            std::uint64_t Current{ 0 }; 
            std::uint64_t Completed{ 0 };
        }
        Position;

        struct
        {
            std::chrono::duration<double> Elapsed{ 0.0 }; 
            std::chrono::duration<double> Now{ 0.0 }; 
            std::chrono::duration<double> Delta{ 0.0 };
        }
        Time;
    };
    // clang-format on

} // namespace N503::Audio::Node
