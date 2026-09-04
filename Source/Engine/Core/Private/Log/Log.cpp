// Copyright (c) Simon Kirsch 2026.

#include "Log/Log.h"

#include <iostream>


namespace {
    constexpr char const* GetSeverityName(Log::Severity level) noexcept {
        switch (level) {
            case Log::Severity::Fatal:
                return "Fatal";
            case Log::Severity::Error:
                return "Error";
            case Log::Severity::Warning:
                return "Warning";
            case Log::Severity::Info:
                return "Info";
            case Log::Severity::Verbose:
                return "Verbose";
            case Log::Severity::Debug:
                return "Debug";
        }

        return "<Unknown>";
    }

    constexpr char const* GetSeverityColor(Log::Severity level) noexcept {
        switch (level) {
            case Log::Severity::Fatal:
                return "\033[4;91m";
            case Log::Severity::Error:
                return "\033[1;31m";
            case Log::Severity::Warning:
                return "\033[1;33m";
            case Log::Severity::Info:
                return "\033[39m";
            case Log::Severity::Verbose:
                return "\033[2;39m";
            case Log::Severity::Debug:
                return "\033[3;36m";
        }
        return "\033[0m";
    }
}

namespace {
    struct LogState {
        Log::Severity minSeverity = Log::Severity::Info;
    } State;
}

bool Log::Initialize() {
    // TODO: init file writing.
    return true;
}

void Log::Shutdown() {
    // TODO: wait on file writing queue.
}

void Log::SetMinSeverity(Severity severity) {
    State.minSeverity = severity;
}

void Log::Write_Impl(Log::Severity severity, std::string_view category, std::string_view message) {
    if (static_cast<u8>(severity) > static_cast<u8>(State.minSeverity)) return;

    std::string output = std::format("{}[{}] [{}] : {}\033[m\n", GetSeverityColor(severity), GetSeverityName(severity), category, message);
    std::cout << output;
}

void Log::WriteRaw_Impl(std::string_view message) {
    std::cout << message;
}

DEFINE_LOG_CATEGORY(LogCore);
DEFINE_LOG_CATEGORY(LogTemp);
