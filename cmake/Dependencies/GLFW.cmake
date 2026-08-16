FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.5.1
)

set(GLFW_BUILD_DOCS OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_X11 OFF)
set(GLFW_BUILD_COCOA OFF)
set(GLFW_BUILD_WIN32 OFF)
set(GLFW_BUILD_WAYLAND OFF)
if (APPLE)
    set(GLFW_BUILD_COCOA ON)
elseif (WIN32)
    set(GLFW_BUILD_WIN32 ON)
elseif (LINUX)
    set(GLFW_BUILD_WAYLAND ON)
else ()
    message(FATAL_ERROR "The detected platform is not supported!")
endif ()

FetchContent_MakeAvailable(glfw)