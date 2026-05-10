# Engine Subproject

# - - - - - - - - - - - -
# Third-party Dependencies
# - - - - - - - - - - - -

include(${CMAKE_SOURCE_DIR}/cmake/Dependencies.cmake)

# - - - - - - - - - - - -
# Actual Engine Project
# - - - - - - - - - - - -

add_subdirectory(Core)
add_subdirectory(Platform)
add_subdirectory(RHI)
add_subdirectory(Engine)



