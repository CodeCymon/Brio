# Engine Subproject

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(Engine)


# - - - - - - - - - - - -
# Third-party Dependencies
# - - - - - - - - - - - -

#find_package(Vulkan REQUIRED)

include(FetchContent)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
set(GLFW_BUILD_X11 OFF)
set(GLFW_BUILD_COCOA OFF)
set(GLFW_BUILD_WIN32 OFF)
set(GLFW_BUILD_WAYLAND OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
if (APPLE)
    set(GLFW_BUILD_COCOA ON)
elseif (WIN32)
    set(GLFW_BUILD_WIN32 ON)
else ()
    set(GLFW_BUILD_WAYLAND ON)
endif ()
FetchContent_MakeAvailable(glfw)
