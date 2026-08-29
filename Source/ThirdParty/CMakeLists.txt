add_library(VMA INTERFACE)
add_library(Brio::VMA ALIAS VMA)

target_include_directories(VMA
    INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}/VMA
)