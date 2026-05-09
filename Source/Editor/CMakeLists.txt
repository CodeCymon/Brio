# Editor Subproject

add_executable(Editor main.cpp)

target_link_libraries(Editor
    PRIVATE
        Core
        Engine
)

target_compile_definitions(Editor PRIVATE
        $<$<CONFIG:Debug>:BUILD_DEBUG=1;BUILD_RELEASE=0>
        $<$<CONFIG:Release>:BUILD_RELEASE=1;BUILD_DEBUG=0>
)

if (WIN32)
    target_compile_definitions(Editor PRIVATE
            PLATFORM_WINDOWS=1 PLATFORM_LINUX=0 PLATFORM_MACOS=0)
elseif (APPLE)
    target_compile_definitions(Editor PRIVATE
            PLATFORM_WINDOWS=0 PLATFORM_LINUX=0 PLATFORM_MACOS=1)
elseif (UNIX)
    target_compile_definitions(Editor PRIVATE
            PLATFORM_WINDOWS=0 PLATFORM_LINUX=1 PLATFORM_MACOS=0)
endif()
