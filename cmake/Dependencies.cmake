include_guard(GLOBAL)
include(FetchContent)

# Project Dependencies:
# GLFW 3.5.1
# Vulkan 1.4
# VMA 3.4.0

find_package(Vulkan REQUIRED)

add_subdirectory(${CMAKE_SOURCE_DIR}/Source/ThirdParty/VMA)

include(cmake/Dependencies/GLFW.cmake)

