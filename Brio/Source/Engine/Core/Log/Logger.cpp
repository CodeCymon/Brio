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
    }

    return "_____";
}

void Log::WriteToLog_Internal(Level level, std::string_view categoryName, std::string_view message)
{
    std::string output = std::format("[{}] [{}] : {}\n", LevelToString(level), categoryName, message);

    if (level <= Log::Level::Error)
    {
        std::cerr << output;
    } else
    {
        std::cout << output;
    }
}
