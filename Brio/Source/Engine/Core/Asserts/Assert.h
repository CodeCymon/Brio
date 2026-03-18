#pragma once

#include "Common/Defines.h"

#include "Core/Log/Logger.h"

#ifdef _MSC_VER
#include <intrin.h>
#define DEBUG_BREAK() __debugbreak()
#else
#define DEBUG_BREAK() __builtin_trap()
#endif

#if RELEASE_BUILD
#define ASSERT(condition, msg)
#else
#define ASSERT(condition, msg) \
    do { if (!(condition)) { \
        LOG_FATAL(LogAssertion, "Assert failed! [{}:{}]: {}", __FILE__, __LINE__, msg); \
        DEBUG_BREAK(); \
    }} while(0)
#endif