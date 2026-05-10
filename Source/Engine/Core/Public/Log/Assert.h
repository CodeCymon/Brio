# pragma once

#include "Log.h"

#if defined (_MSC_VER)
    #include <intrin.h>
    #define DEBUG_BREAK() __debugbreak()
#else
    #define DEBUG_BREAK() __builtin_trap()
#endif

#if BUILD_DEBUG
    #define ASSERT_MSG(condition, msg, ...) \
        do { \
            if (!(condition)) { \
                LOG_FATAL(LogAssert, "Assertion failed! [{}:{}]: {}", __FILE__, __LINE__, msg __VA_OPT__(,) __VA_ARGS__); \
                DEBUG_BREAK(); \
            } \
        } while (0)

    #define ASSERT(condition) \
        do { \
            if (!(condition)) { \
                LOG_FATAL(LogAssert, "Assertion failed! [{}:{}]", __FILE__, __LINE__); \
                DEBUG_BREAK(); \
            } \
        } while (0)
#else
    #define ASSERT_MSG(condition, msg, ...)
    #define ASSERT(condition)
#endif

