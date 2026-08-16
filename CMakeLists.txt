cmake_minimum_required(VERSION 3.24)
project(Brio LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)
include(cmake/BrioModule.cmake)

# Dependencies
include(cmake/Dependencies.cmake)

# Engine
add_subdirectory(Source/Engine/Core)
add_subdirectory(Source/Engine/Platform)

# Sandbox
add_subdirectory(Source/Sandbox)