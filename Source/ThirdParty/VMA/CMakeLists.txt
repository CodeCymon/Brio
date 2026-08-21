add_library(VMA STATIC
    vma_implementation.cpp
)

target_include_directories(VMA
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)