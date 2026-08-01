add_executable(Sandbox Private/main.cpp)
target_link_libraries(Sandbox PRIVATE Brio::Core)

target_compile_definitions(Sandbox PRIVATE
        $<$<CONFIG:Debug>:BUILD_DEBUG=1;BUILD_RELEASE=0>
        $<$<CONFIG:Release>:BUILD_RELEASE=1;BUILD_DEBUG=0>
)