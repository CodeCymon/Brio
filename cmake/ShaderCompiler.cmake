# Shader Compiler

function(compile_shaders TARGET)
    cmake_parse_arguments(ARG "" "OUTPUT_DIR" "SOURCES" ${ARGN})

    file(MAKE_DIRECTORY ${ARG_OUTPUT_DIR})

    foreach (SHADER_ENTRY ${ARG_SOURCES})
        string(REPLACE ":" ";" SHADER_PARTS ${SHADER_ENTRY})
        list(GET SHADER_PARTS 0 SHADER_PATH)
        list(GET SHADER_PARTS 1 SHADER_STAGE)
        list(GET SHADER_PARTS 2 SHADER_ENTRY_POINT)

        get_filename_component(SHADER_NAME ${SHADER_PATH} NAME)
        if(SHADER_STAGE STREQUAL "vs")
            set(PROFILE "vs_6_6")
        elseif (SHADER_STAGE STREQUAL "ps")
            set(PROFILE "ps_6_6")
        elseif (SHADER_STAGE STREQUAL "cs")
            set(PROFILE "cs_6_6")
        elseif (SHADER_STAGE STREQUAL "lib")
            set(PROFILE "lib_6_6")
        endif ()

        set(OUTPUT "${ARG_OUTPUT_DIR}/${SHADER_NAME}.${SHADER_STAGE}.spv")


        add_custom_command(
                OUTPUT ${OUTPUT}
                COMMAND ${DXC_EXECUTABLE}
                        -T ${PROFILE}
                        -E ${SHADER_ENTRY_POINT}
                        -spirv
                        -fspv-target-env=vulkan1.3
                        -I ${CMAKE_SOURCE_DIR}/Shaders
                        -Fo ${OUTPUT}
                        ${SHADER_PATH}
                DEPENDS ${SHADER_PATH}
                COMMENT "Compiling ${SHADER_NAME} Shader [${SHADER_STAGE}]"
        )

        list(APPEND SHADER_OUTPUTS ${OUTPUT})

    endforeach ()

    add_custom_target(${TARGET}_Shaders DEPENDS ${SHADER_OUTPUTS})
    add_dependencies(${TARGET} ${TARGET}_Shaders)

endfunction()