# Define C++ "Module"

function(add_engine_module MODULE_NAME)
    cmake_parse_arguments(ARG "" "" "PUBLIC_DEPS;PRIVATE_DEPS;SOURCES" ${ARGN})

    add_library(${MODULE_NAME} SHARED ${ARG_SOURCES})

    target_include_directories(${MODULE_NAME}
        PUBLIC 
            ${CMAKE_CURRENT_SOURCE_DIR}/Public
            ${CMAKE_CURRENT_BINARY_DIR}/Public
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/Private
            ${CMAKE_CURRENT_SOURCE_DIR}/Internal
    )

    set_target_properties(${MODULE_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )

    string(TOUPPER ${MODULE_NAME} MODULE_UPPER)
    generate_export_header(${MODULE_NAME}
        EXPORT_MACRO_NAME ${MODULE_UPPER}_API
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/Public/${MODULE_NAME}API.h
    )

    target_compile_definitions(${MODULE_NAME} PRIVATE
        $<$<CONFIG:Debug>:BUILD_DEBUG=1;BUILD_RELEASE=0>
        $<$<CONFIG:Release>:BUILD_RELEASE=1;BUILD_DEBUG=0>
    )

    if (WIN32)
        target_compile_definitions(${MODULE_NAME} PRIVATE
            PLATFORM_WINDOWS=1 PLATFORM_LINUX=0 PLATFORM_MACOS=0)
    elseif (APPLE)
        target_compile_definitions(${MODULE_NAME} PRIVATE
            PLATFORM_WINDOWS=0 PLATFORM_LINUX=0 PLATFORM_MACOS=1)
    elseif (UNIX)
        target_compile_definitions(${MODULE_NAME} PRIVATE
            PLATFORM_WINDOWS=0 PLATFORM_LINUX=1 PLATFORM_MACOS=0)
    endif()

    if(ARG_PUBLIC_DEPS)
        target_link_libraries(${MODULE_NAME} PUBLIC ${ARG_PUBLIC_DEPS})
    endif()

    if(ARG_PRIVATE_DEPS)
        target_link_libraries(${MODULE_NAME} PRIVATE ${ARG_PRIVATE_DEPS})
    endif()

endfunction()
