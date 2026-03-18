#pragma once

#include "Types.h"

#if defined(USE_EXPORT)
#if defined(_MSC_VER)
#define BAPI __declspec(dllexport)
#elif defined(__GNUC__)
#define BAPI __attribute__((visibility("default")))
#else
#define BAPI
#pragma warning Unknown dynamic link export semantics.
#endif
#else
#if defined(_MSC_VER)
#define BAPI __declspec(dllimport)
#elif defined(__GNUC__)
#define BAPI
#else
#define BAPI
#pragma warning Unknown dynamic link import semantics.
#endif
#endif



#if defined(NDEBUG)
#define DEBUG_BUILD 0
#define RELEASE_BUILD 1
#else
#define DEBUG_BUILD 1
#define RELEASE_BUILD 0
#endif



#define PLATFORM_WINDOWS 0
#define PLATFORM_LINUX 0
#define PLATFORM_MACOS 0

#if defined (_WIN32)
#undef PLATFORM_WINDOWS
#define PLATFORM_WINDOWS 1
#endif

#if defined (__linux__)
#undef PLATFORM_LINUX
#define PLATFORM_LINUX 1
#endif

#if defined (__APPLE__)
#undef PLATFORM_MACOS
#define PLATFORM_MACOS 1
#endif

