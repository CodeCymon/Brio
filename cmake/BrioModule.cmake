include_guard(GLOBAL)
include(GenerateExportHeader)

# brio_add_module(
#     NAME <ModuleName>
#     SOURCES <Private/File1.cpp> [...]
#     [PUBLIC_LINK <target> ...]
#     [PRIVATE_LINK <target> ...]
# )
#
# Expects Public/Private split:
#   <Module>/Public/**/*.h      - headers other modules may need to include
#   <Module>/Private/**/*       - implementation + module-private headers
function(brio_add_module)
    set(singleValueArgs NAME)
    set(options INTERFACE)
    set(multiValueArgs SOURCES PUBLIC_LINK PRIVATE_LINK)

    cmake_parse_arguments(
        MOD
        "${options}"
        "${singleValueArgs}"
        "${multiValueArgs}"
        ${ARGN}
    )

    if(NOT MOD_NAME)
        message(FATAL_ERROR "brio_add_module: NAME is required!")
    endif()

    if(MOD_INTERFACE)
        add_library(${MOD_NAME} INTERFACE)
        add_library(Brio::${MOD_NAME} ALIAS ${MOD_NAME})

        target_include_directories(${MOD_NAME}
            INTERFACE
                $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
        )

        if(MOD_PUBLIC_LINK)
            target_link_libraries(${MOD_NAME}
                INTERFACE
                    ${MOD_PUBLIC_LINK}
            )
        endif ()

        return()
    endif ()

    if(NOT MOD_SOURCES)
        message(FATAL_ERROR "brio_add_module(${MOD_NAME}): SOURCES are required")
    endif()

    add_library(${MOD_NAME} SHARED ${MOD_SOURCES})
    add_library(Brio::${MOD_NAME} ALIAS ${MOD_NAME})

    string(TOUPPER ${MOD_NAME} MOD_NAME_UPPER)
    generate_export_header(${MOD_NAME}
        EXPORT_MACRO_NAME ${MOD_NAME_UPPER}_API
        EXPORT_FILE_NAME ${CMAKE_CURRENT_BINARY_DIR}/Generated/${MOD_NAME}API.h
    )

    target_include_directories(${MOD_NAME}
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Public>
            $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/Generated>
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Private>
    )

    target_compile_features(${MOD_NAME} PUBLIC cxx_std_20)

    target_compile_definitions(${MOD_NAME} PRIVATE
        $<$<CONFIG:Debug>:BUILD_DEBUG=1;BUILD_RELEASE=0>
        $<$<CONFIG:Release>:BUILD_DEBUG=0;BUILD_RELEASE=1>
    )

    if(WIN32)
        target_compile_definitions(${MOD_NAME} PRIVATE
                PLATFORM_WINDOWS=1 PLATFORM_LINUX=0 PLATFORM_MACOS=0)
    elseif(APPLE)
        target_compile_definitions(${MOD_NAME} PRIVATE
                PLATFORM_WINDOWS=0 PLATFORM_LINUX=0 PLATFORM_MACOS=1)
    elseif(UNIX)
        target_compile_definitions(${MOD_NAME} PRIVATE
                PLATFORM_WINDOWS=0 PLATFORM_LINUX=1 PLATFORM_MACOS=0)
    endif()

    set_target_properties(${MOD_NAME} PROPERTIES
        CXX_VISIBILITY_PRESET hidden
        VISIBILITY_INLINES_HIDDEN ON
    )

    if(MOD_PUBLIC_LINK)
        target_link_libraries(${MOD_NAME} PUBLIC ${MOD_PUBLIC_LINK})
    endif()

    if(MOD_PRIVATE_LINK)
        target_link_libraries(${MOD_NAME} PRIVATE ${MOD_PRIVATE_LINK})
    endif()
endfunction()