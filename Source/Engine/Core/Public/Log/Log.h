# pragma once

#include "CoreAPI.h"

#include "Types/CoreTypes.h"

#include <format>

namespace FLog {
    
    struct CORE_API FCategory {
        explicit FCategory(char const* InName) : name(InName) {}
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
        FLog::WriteToLog_Implementation(level, category, std::format(fmt, std::forward<Args>(args)...).c_str());
    }
}

#define LOG_FATAL(category, message, ...) \
    do { \
        FLog::WriteToLog(FLog::Level::Fatal, category.name, message __VA_OPT__(,) __VA_ARGS__); \
        exit(1); \
    } while(0);

#define LOG_ERROR(category, message, ...) FLog::WriteToLog(FLog::Level::Error, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_WARNING(category, message, ...) FLog::WriteToLog(FLog::Level::Warning, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_INFO(category, message, ...) FLog::WriteToLog(FLog::Level::Info, category.name, message __VA_OPT__(,) __VA_ARGS__)

#if BUILD_DEBUG
#define LOG_DEBUG(category, message, ...) FLog::WriteToLog(FLog::Level::Debug, category.name, message __VA_OPT__(,) __VA_ARGS__)

#define LOG_DETAIL(category, message, ...) FLog::WriteToLog(FLog::Level::Detail, category.name, message __VA_OPT__(,) __VA_ARGS__)
#else
#define LOG_DEBUG(category, message, ...)

#define LOG_DETAIL(category, message, ...)
#endif


#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) \
    struct CORE_API FLogCategory##CategoryName : public FLog::FCategory \
    { \
        inline FLogCategory##CategoryName() : FLog::FCategory(#CategoryName) {} \
    }; \
    extern CORE_API FLogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY(CategoryName) \
    CORE_API FLogCategory##CategoryName CategoryName;

#define DEFINE_LOG_CATEGORY_STATIC(CategoryName) \
    static struct FLogCategory##CategoryName : public FLog::FCategory \
    { \
        inline FLogCategory##CategoryName() : FLog::FCategory(#CategoryName) {} \
    } CategoryName;

#define DEFINE_LOG_CATEGORY_MODULE(CategoryName) \
    struct FLogCategory##CategoryName : public FLog::FCategory \
    { \
        inline FLogCategory##CategoryName() : FLog::FCategory(#CategoryName) {} \
    }; \
    inline FLogCategory##CategoryName CategoryName;

DECLARE_LOG_CATEGORY_EXTERN(LogCore);
DECLARE_LOG_CATEGORY_EXTERN(LogTemp);
DECLARE_LOG_CATEGORY_EXTERN(LogAssert)
