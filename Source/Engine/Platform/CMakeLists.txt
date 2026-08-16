brio_add_module(
    NAME Platform
    SOURCES
        Private/Window.cpp
        Private/Platform.cpp
    PUBLIC_LINK
        Core
    PRIVATE_LINK
        glfw
)