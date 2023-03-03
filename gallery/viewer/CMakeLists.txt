set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})
get_filename_component(ENTRY ${CMAKE_CURRENT_LIST_DIR} NAME)

file(GLOB INCLUDE_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

add_subdirectory(${PROJECT_ROOT}/shaders)
add_subdirectory(${PROJECT_ROOT}/vkx)

source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})
source_group("shaders" FILES ${SHADER_LIST})

create_gallery_entry(
    TARGET ${ENTRY} EXECUTABLE SKIP_FLAGS
    SOURCES ${INCLUDE_LIST} ${SOURCE_LIST} ${SHADER_LIST}
    ROOT ${PROJECT_ROOT}
    FOLDER "${ENTRY}"
    LIB vkx
    )
create_spv_target(TARGET ${ENTRY}
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/spv"
    SOURCES ${SHADER_LIST}
    FOLDER "${ENTRY}"
    )
