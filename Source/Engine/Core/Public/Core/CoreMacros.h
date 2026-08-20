// Copyright (c) Simon Kirsch 2026.

#pragma once

#define NON_COPYABLE(ClassName) \
    ClassName(ClassName const&) = delete; \
    ClassName& operator=(ClassName const&) = delete;

#define NON_MOVEABLE(ClassName) \
    ClassName(ClassName&&) = delete; \
    ClassName& operator=(ClassName&&) = delete;

