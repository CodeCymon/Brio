// Copyright (c) Simon Kirsch 2026.

#include "Log/Assert.h"

#include "Log/Log.h"

namespace Assert {
    void ReportAssertionFailure(std::string_view expression, std::string_view file, i32 line) {
        Log::WriteRaw("\033[4;91mAssertion failed: '{}' in {}:{}\033[0m\n", expression, file, line);
    }

    void ReportAssertionFailure(std::string_view expression, std::string_view message, std::string_view file, i32 line) {
        Log::WriteRaw("\033[4;91mAssertion failed: '{}' with message: '{}' in {}:{}\033[0m\n", expression, message, file, line);
    }
}
