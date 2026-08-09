// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "CoreAPI.h"
#include "Core/CoreTypes.h"

#include <format>

namespace Log {
    struct CORE_API Category {
        explicit Category(char const* inName) : name(inName) {};
        char const* name {nullptr};
    };

    enum class Level : u8 {
        Fatal = 0,
        Error,
        Warning,
        Info,
        Verbose,
        Debug,
    };

    CORE_API void Initialize();
    CORE_API void Shutdown();

    CORE_API void Write_Impl(Log::Level level, std::string_view category, std::string_view message);
    CORE_API void WriteRaw_Impl(std::string_view message);

    template<typename... Args>
    inline void WriteRaw(std::format_string<Args...> fmt, Args&&... args) {
        Log::WriteRaw_Impl(std::format(fmt, std::forward<Args>(args)...));
    }

    template<typename... Args>
    inline void Write(Log::Level level, std::string_view category, std::format_string<Args...> fmt, Args&&... args) {
        Log::Write_Impl(level, category, std::format(fmt, std::forward<Args>(args)...));
    }
}

#define LOG_FATAL(category, message, ...) \
    do { \
        Log::Write(Log::Level::Fatal, category.name, message __VA_OPT__(,) __VA_ARGS__); \
        exit(1); \
    } while (0);

#define LOG_ERROR(category, message, ...) Log::Write(Log::Level::Error, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARNING(category, message, ...) Log::Write(Log::Level::Warning, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(category, message, ...) Log::Write(Log::Level::Info, category.name, message __VA_OPT__(,) __VA_ARGS__)

#if BUILD_DEBUG
#define LOG_VERBOSE(category, message, ...) Log::Write(Log::Level::Verbose, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_DEBUG(category, message, ...) Log::Write(Log::Level::Debug, category.name, message __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_VERBOSE(category, message, ...) ((void)0)

#define LOG_DEBUG(category, message, ...) ((void)0)
#endif

#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) \
    struct CORE_API FLogCategory##CategoryName : public Log::Category \
    { \
        inline FLogCategory##CategoryName() : Log::Category(#CategoryName) {} \
    }; \
    extern CORE_API FLogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY(CategoryName) \
    CORE_API FLogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY_STATIC(CategoryName) \
    static struct FLogCategory##CategoryName : public Log::Category \
    { \
        inline FLogCategory##CategoryName() : Log::Category(#CategoryName) {} \
    } CategoryName;

#define DEFINE_LOG_CATEGORY_MODULE(CategoryName) \
    struct FLogCategory##CategoryName : public Log::Category \
    { \
        inline FLogCategory##CategoryName() : Log::Category(#CategoryName) {} \
    }; \
    inline FLogCategory##CategoryName CategoryName;

DECLARE_LOG_CATEGORY_EXTERN(LogCore);
DECLARE_LOG_CATEGORY_EXTERN(LogTemp);
DECLARE_LOG_CATEGORY_EXTERN(LogTest);