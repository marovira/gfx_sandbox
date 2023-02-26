function(create_gallery_entry)
    set(options EXECUTABLE LIBRARY USE_PUBLIC SKIP_FLAGS)
    set(oneValueArgs TARGET ROOT FOLDER)
    set(multiValueArgs SOURCES LIB DEFINES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

#================================
# Compiler flags.
#================================
    if (MSVC)
        set(COMPILER_FLAGS /W4 /WX /MP /EHsc)
        set(DEBUG_FLAGS "$<$<CONFIG:Debug>:/ZI>")
        set(COMPILER_DEFINITIONS -DNOMINMAX)
    else()
        set(COMMON_FLAGS -Wall -Wextra -pedantic -Werror)
        if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
            set(COMPILER_FLAGS ${COMMON_FLAGS})
        else()
            set(COMPILER_FLAGS ${COMMON_FLAGS} -stdlib=libstdc++)
        endif()
        set(DEBUG_FLAGS "$<$<CONFIG:Debug>:-g>")
        set(COMPILER_DEFINITIONS)
    endif()

    if (NOT ${ARG_EXECUTABLE} AND NOT ${ARG_LIBRARY})
        message(FATAL_ERROR 
            "error: target type not provided. Did you forget to add EXECUTABLE or LIBRARY?")
    endif()

    if (${ARG_EXECUTABLE})
        add_executable(${ARG_TARGET} ${ARG_SOURCES})
    elseif(${ARG_LIBRARY})
        add_library(${ARG_TARGET} ${ARG_SOURCES})
    endif()

    if (${ARG_USE_PUBLIC})
        target_include_directories(${ARG_TARGET} PUBLIC ${ARG_ROOT})
        target_link_libraries(${ARG_TARGET} PUBLIC ${ARG_LIB})

        if (NOT ${ARG_SKIP_FLAGS})
            target_compile_features(${ARG_TARGET} PUBLIC cxx_std_20)
            target_compile_options(${ARG_TARGET} PUBLIC ${COMPILER_FLAGS} ${DEBUG_FLAGS})
            target_compile_definitions(${ARG_TARGET} PUBLIC 
                ${COMPILER_DEFINITIONS}
                ${ARG_DEFINES}
                )
        endif()
    else()
        target_include_directories(${ARG_TARGET} PRIVATE ${ARG_ROOT})
        target_link_libraries(${ARG_TARGET} PRIVATE ${ARG_LIB})

        if (NOT ${ARG_SKIP_FLAGS})
            target_compile_features(${ARG_TARGET} PRIVATE cxx_std_20)
            target_compile_options(${ARG_TARGET} PRIVATE ${COMPILER_FLAGS} ${DEBUG_FLAGS})
            target_compile_definitions(${ARG_TARGET} PRIVATE 
                ${COMPILER_DEFINITIONS}
                ${ARG_DEFINES}
                )
        endif()
    endif()

    set_target_properties(${ARG_TARGET} PROPERTIES FOLDER ${ARG_FOLDER})
endfunction()

function(create_spv_target)
    set(options)
    set(oneValueArgs TARGET OUTPUT FOLDER)
    set(multiValueArgs SOURCES)
    cmake_parse_arguments(ARG "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

    set(SPV_BUILD_ROOT ${ARG_OUTPUT})
    file(MAKE_DIRECTORY ${SPV_BUILD_ROOT})

    foreach (GLSL ${ARG_SOURCES})
        get_filename_component(FILE_NAME ${GLSL} NAME)
        set(SPIRV "${SPV_BUILD_ROOT}/${FILE_NAME}.spv")
        add_custom_command(
            OUTPUT ${SPIRV}
            COMMAND Vulkan::glslangValidator -V ${GLSL} -o ${SPIRV}
            DEPENDS ${GLSL}
            )
        list(APPEND SPIRV_BINARY_FILES ${SPIRV})
    endforeach()

    set(SPV_TARGET ${ARG_TARGET}_spv)

    add_custom_target(${SPV_TARGET} DEPENDS ${SPIRV_BINARY_FILES})
    add_custom_command(TARGET ${SPV_TARGET} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${SPV_BUILD_ROOT}
        "$<TARGET_FILE_DIR:${ARG_TARGET}>/spv"
        )
    set_target_properties(${SPV_TARGET} PROPERTIES FOLDER ${ARG_FOLDER})
endfunction()
