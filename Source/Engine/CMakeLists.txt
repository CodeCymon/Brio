# Engine Subproject

# - - - - - - - - - - - -
# Third-party Dependencies
# - - - - - - - - - - - -

include(${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake)
set(DXC_EXECUTABLE "/home/cymon/Library/vulkanSDK/1.4.350.1/x86_64/bin/dxc")

# - - - - - - - - - - - -
# Actual Engine Project
# - - - - - - - - - - - -

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(RHI)
add_subdirectory(Engine)
add_subdirectory(Renderer)


