

include(FetchContent)


find_package(Vulkan REQUIRED)

FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG 3.4
)
if (APPLE)
    set(GLFW_BUILD_COCOA ON)
elseif (WIN32)
    set(GLFW_BUILD_WIN32 OFF)
else ()
    set(GLFW_BUILD_WAYLAND OFF)
endif ()
set(GLFW_BUILD_X11 OFF)
set(GLFW_BUILD_EXAMPLES OFF)
set(GLFW_BUILD_TESTS OFF)
set(GLFW_BUILD_DOCS OFF)
FetchContent_MakeAvailable(glfw)



target_link_libraries(Engine
        PRIVATE
        glfw
        Vulkan::Vulkan
)
