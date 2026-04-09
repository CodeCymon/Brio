#include "Logger.h"

#include <iostream>


void Log::Initialize()
{
    // TODO: Initialize file writing
}

void Log::Shutdown()
{
    // TODO: flush log and finish file-write
}

constexpr const char* LevelToString(Log::Level level) {
    switch (level) {
    case Log::Level::Fatal:
        return "FATAL";
    case Log::Level::Error:
        return "ERROR";
    case Log::Level::Warning:
        return "WARNING";
    case Log::Level::Info:
        return "INFO";
    case Log::Level::Debug:
        return "DEBUG";
    case Log::Level::Detail:
        return "DETAIL";
    }

    return "_____";
}

constexpr const char* LevelToColorCode(Log::Level level) {
    switch (level) {
        case Log::Level::Fatal:
            return "\033[1;31m";
        case Log::Level::Error:
            return "\033[31m";
        case Log::Level::Warning:
            return "\033[33m";
        case Log::Level::Info:
            return "\033[39m";
        case Log::Level::Debug:
            return "\033[36m";
        case Log::Level::Detail:
            return "\033[35m";
    }
    return "\033[0m";
}

void Log::WriteToLog_Internal(Level level, std::string_view categoryName, std::string_view message)
{
    std::string output = std::format("{}[{}] [{}] : {}\033[0m\n", LevelToColorCode(level), LevelToString(level), categoryName, message);

    if (level <= Log::Level::Error)
    {
        std::cerr << output;
    } else
    {
        std::cout << output;
    }
}
