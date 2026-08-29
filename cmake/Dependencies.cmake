include_guard(GLOBAL)
include(FetchContent)

# Project Dependencies:
# GLFW 3.5.1
# Vulkan 1.4 (sdk: 1.4.357.1)
# slang 2026.13.1 (via: vulkan sdk)
# VMA 3.4.0

find_package(Vulkan REQUIRED)

find_package(slang CONFIG REQUIRED
        HINTS "$ENV{VULKAN_SDK}/lib/cmake"
)

add_subdirectory(${CMAKE_SOURCE_DIR}/Source/ThirdParty)

include(cmake/Dependencies/GLFW.cmake)

