cmake_minimum_required(VERSION 3.30)
project(Brio)

set(CMAKE_CXX_STANDARD 20)


foreach (CONFIG Debug Release)
    string(TOUPPER ${CONFIG} CONFIG_UPPER)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_SOURCE_DIR}/Build/${CONFIG}/bin")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_SOURCE_DIR}/Build/${CONFIG}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_${CONFIG_UPPER} "${CMAKE_SOURCE_DIR}/Build/${CONFIG}/bin")
endforeach ()

add_subdirectory(Brio/Source/Editor)
add_subdirectory(Brio/Source/Engine)
