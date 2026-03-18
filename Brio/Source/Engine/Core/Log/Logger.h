#pragma once

#include "Common/Defines.h"

#include "LogCategories.h"

#include <format>
#include <string_view>

#define LOG_WARN_ENABLED 1
#define LOG_INFO_ENABLED 1
#define LOG_DEBUG_ENABLED 1

#if RELEASE_BUILD
#ifdef LOG_DEBUG_ENABLED
#undef LOG_DEBUG_ENABLED
#endif
#define LOG_DEBUG_ENABLED 0
#endif

namespace Log {

    enum class Level : u8
    {
        Fatal = 0,
        Error,
        Warning,
        Info,
        Debug,
    };

    void Initialize();
    void Shutdown();

    /// @warning This function in for internal usage by the Logger only! Do not call it manually!
    BAPI void WriteToLog_Internal(Level level, std::string_view categoryName, std::string_view message);

    template<typename... Args>
    inline void WriteToLog(Level level, std::string_view categoryName, std::format_string<Args...> fmt, Args&&... args)
    {
        Log::WriteToLog_Internal(level, categoryName, std::format(fmt, std::forward<Args>(args)...));
    };
}

#define LOG_FATAL(category, message, ...) WriteToLog(Log::Level::Fatal, category.name, message, ##__VA_ARGS__);

#define LOG_ERROR(category, message, ...) WriteToLog(Log::Level::Error, category.name, message, ##__VA_ARGS__);

#if LOG_WARN_ENABLED
#define LOG_WARN(category, message, ...) WriteToLog(Log::Level::Warning, category.name, message, ##__VA_ARGS__);
#else
#define BWARN(category, message, ...)
#endif

#if LOG_INFO_ENABLED
#define LOG_INFO(category, message, ...) WriteToLog(Log::Level::Info, category.name, message, ##__VA_ARGS__);
#else
#define LOG_INFO(category, message, ...)
#endif

#if LOG_DEBUG_ENABLED
#define LOG_DEBUG(category, message, ...) WriteToLog(Log::Level::Debug, category.name, message, ##__VA_ARGS__);
#else
#define LOG_DEBUG(category, message, ...)
#endif