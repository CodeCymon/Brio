add_executable(Editor)

target_sources(Editor
        PRIVATE
        main.cpp
)

target_link_libraries(Editor
        PRIVATE
        Engine
)

target_include_directories(Editor
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
        "${CMAKE_CURRENT_SOURCE_DIR}/../Engine"
)
