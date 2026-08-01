#include "Log/Log.h"

#include <iostream>


namespace {
    constexpr char const* GetLogLevelName(Log::Level level) noexcept {
        switch (level) {
            case Log::Level::Fatal:
                return "Fatal";
            case Log::Level::Error:
                return "Error";
            case Log::Level::Warning:
                return "Warning";
            case Log::Level::Info:
                return "Info";
            case Log::Level::Verbose:
                return "Verbose";
            case Log::Level::Debug:
                return "Debug";
        }

        return "<Unknown>";
    }

    constexpr char const* GetLogLevelColor(Log::Level level) noexcept {
        switch (level) {
            case Log::Level::Fatal:
                return "\033[4;91m";
            case Log::Level::Error:
                return "\033[1;31m";
            case Log::Level::Warning:
                return "\033[1;33m";
            case Log::Level::Info:
                return "\033[39m";
            case Log::Level::Verbose:
                return "\033[2;39m";
            case Log::Level::Debug:
                return "\033[3;36m";
        }
        return "\033[0m";
    }
}

void Log::Initialize() {
    // TODO: init file writing.
}

void Log::Shutdown() {
    // TODO: wait on file writing queue.
}

void Log::Write_Impl(Log::Level level, std::string_view category, std::string_view message) {
    std::string output = std::format("{}[{}] [{}] : {}\033[m\n", GetLogLevelColor(level), GetLogLevelName(level), category, message);
    std::cout << output;
}

void Log::WriteRaw_Impl(std::string_view message) {
    std::cout << message;
}

DEFINE_LOG_CATEGORY(LogCore);
DEFINE_LOG_CATEGORY(LogTemp);