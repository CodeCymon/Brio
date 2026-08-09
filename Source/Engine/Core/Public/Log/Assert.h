// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "CoreAPI.h"
#include "Core/CoreTypes.h"

#include <string_view>


#if defined (_MSC_VER)
    #include <intrin.h>
    #define DEBUG_BREAK() __debugbreak()
#elif defined (__GNUC__)
    #define DEBUG_BREAK() __asm__("int $3")
#elif defined (__clang__)
    #define DEBUG_BREAK() __builtin_debugtrap()
#endif

namespace Assert {
    CORE_API void ReportAssertionFailure(std::string_view expression, std::string_view file, i32 line);
}

#if BUILD_DEBUG
    #define ASSERT(expression) \
        do { \
            if (!(expression)) { \
                Assert::ReportAssertionFailure(#expression, __FILE__, __LINE__); \
                DEBUG_BREAK(); \
            } \
        } while(0);
#else
    #define ASSERT(expression) ((void)0)
#endif