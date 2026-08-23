// Copyright (c) Simon Kirsch 2026.

#pragma once

#include "CoreMinimal.h"

#include "RHIResource.h"


class VulkanDeletionQueue {
public:
    void Enqueue(RHIResource* resource, u64 readyAtTimelineValue);

    void Flush(u64 completedTimelineValue);

    void FlushAll();

private:
    struct Entry {
        RHIResource* resource;
        u64 readyAt;
    };
    Array<Entry> pending;
};
