add_library(Engine SHARED)

target_compile_definitions(Engine
        PRIVATE
        USE_EXPORT
)

set_target_properties(Engine
        PROPERTIES
        CXX_VISIBILITY_PRESET hidden
)

file(GLOB_RECURSE HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")

target_sources(Engine
        PUBLIC
        ${HEADERS}
        PRIVATE
        ${SOURCES}
)

target_include_directories(Engine
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

include(dependencies.cmake)