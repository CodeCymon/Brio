# pragma once

#include "CoreAPI.h"

#include "Types/CoreTypes.h"

#include <format>

namespace Log {
    
    struct CORE_API Category {
        explicit Category(char const* InName) : name(InName) {}
        char const* name { nullptr };
    };

    enum class Level : u8 {
        Fatal = 0,
        Error,
        Warning,
        Info,
        Debug,
        Detail,
    };

    CORE_API void Initialize();
    CORE_API void Shutdown();

    // @warning internal use only - do not call this function directly! Prefer the Macros instead.
    CORE_API void WriteToLog_Implementation(Level level, char const* category, char const* message);

    template<typename... Args>
    void WriteToLog(Level level, char const* category, std::format_string<Args...> fmt, Args&&... args) {
        Log::WriteToLog_Implementation(level, category, std::format(fmt, std::forward<Args>(args)...).c_str());
    }
}

#define LOG_FATAL(category, message, ...) \
    do { \
        Log::WriteToLog(Log::Level::Fatal, category.name, message __VA_OPT__(,) __VA_ARGS__); \
        exit(1); \
    } while(0);

#define LOG_ERROR(category, message, ...) Log::WriteToLog(Log::Level::Error, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARNING(category, message, ...) Log::WriteToLog(Log::Level::Warning, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(category, message, ...) Log::WriteToLog(Log::Level::Info, category.name, message __VA_OPT__(,) __VA_ARGS__)

#if BUILD_DEBUG
#define LOG_DEBUG(category, message, ...) Log::WriteToLog(Log::Level::Debug, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_DETAIL(category, message, ...) Log::WriteToLog(Log::Level::Detail, category.name, message __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_DEBUG(category, message, ...)

#define LOG_DETAIL(category, message, ...)
#endif


#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) \
    struct CORE_API LogCategory##CategoryName : public Log::Category \
    { \
        inline LogCategory##CategoryName() : Log::Category(#CategoryName) {} \
    }; \
    extern CORE_API LogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY(CategoryName) \
    CORE_API LogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY_STATIC(CategoryName) \
    static struct LogCategory##CategoryName : public Log::Category \
    { \
        inline LogCategory##CategoryName() : Log::Category(#CategoryName) {} \
    } CategoryName;

DECLARE_LOG_CATEGORY_EXTERN(LogCore);
DECLARE_LOG_CATEGORY_EXTERN(LogTemp);
DECLARE_LOG_CATEGORY_EXTERN(LogAssertion)
