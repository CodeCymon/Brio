// Copyright (c) Simon Kirsch 2026.

#pragma once
#include "PlatformAPI.h"
#include "Log/Log.h"

DECLARE_LOG_CATEGORY_EXTERN(LogPlatform);

class PLATFORM_API Platform {
public:
    static bool Initialize();
    static void Shutdown();

    static void PollEvents();
};