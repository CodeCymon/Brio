brio_add_module(
    NAME RHI
    SOURCES
        Private/Empty.cpp
    PUBLIC_LINK
        Brio::Core
    PRIVATE_LINK
        Brio::Platform
)