#include "Log/Log.h"

#include <iostream>

namespace {
    constexpr const char* LevelToString(FLog::Level level) {
        switch (level) {
        case FLog::Level::Fatal:
            return "FATAL";
        case FLog::Level::Error:
            return "ERROR";
        case FLog::Level::Warning:
            return "WARNING";
        case FLog::Level::Info:
            return "INFO";
        case FLog::Level::Debug:
            return "DEBUG";
        case FLog::Level::Detail:
            return "DETAIL";
        }

        return "_____";
    }

    constexpr const char* LevelToColorCode(FLog::Level level) {
        switch (level) {
            case FLog::Level::Fatal:
                return "\033[1;31m";
            case FLog::Level::Error:
                return "\033[31m";
            case FLog::Level::Warning:
                return "\033[33m";
            case FLog::Level::Info:
                return "\033[39m";
            case FLog::Level::Debug:
                return "\033[36m";
            case FLog::Level::Detail:
                return "\033[35m";
        }
        return "\033[0m";
    }
}

void FLog::Initialize() {}

void FLog::Shutdown() {}

void FLog::WriteToLog_Implementation(Level level, char const* category, char const* message) {
    std::string output = std::format("{}[{}] [{}] : {}\033[0m\n", LevelToColorCode(level), LevelToString(level), category, message);

    std::cout << output;
}

DEFINE_LOG_CATEGORY(LogCore);
DEFINE_LOG_CATEGORY(LogTemp);
DEFINE_LOG_CATEGORY(LogAssert)
