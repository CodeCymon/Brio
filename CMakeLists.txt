# Top Level Project

cmake_minimum_required(VERSION 3.30)

project(BrioEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_EXTENSIONS OFF)

include(cmake/Module.cmake)
include(GenerateExportHeader)

add_subdirectory(Source/Engine)
add_subdirectory(Source/Editor)