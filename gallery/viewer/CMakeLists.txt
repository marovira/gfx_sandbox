set(PROJECT_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(INCLUDE_LIST
    )

set(SOURCE_LIST
    ${PROJECT_ROOT}/main.cpp
    )
set(SHADER_LIST
    )

source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})
source_group("shaders" FILES ${SHADER_LIST})

set(TARGET_NAME viewer)

create_gallery_entry(
    TARGET ${TARGET_NAME} EXECUTABLE
    SOURCES ${INCLUDE_LIST} ${SOURCE_LIST} ${SHADER_LIST}
    ROOT ${PROJECT_ROOT}
    FOLDER "${TARGET_NAME}"
    LIB 
        Vulkan::Vulkan
        zeus::zeus
        glm::glm
        vk-bootstrap::vk-bootstrap
        VulkanMemoryAllocator
    )
create_spv_target(TARGET viewer
    OUTPUT "${CMAKE_CURRENT_BINARY_DIR}/spv"
    SOURCES ${SHADER_LIST}
    FOLDER "${TARGET_NAME}"
    )
