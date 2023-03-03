file(GLOB INCLUDE_LIST CONFIGURE_DEPENDS "*.hpp")
file(GLOB SOURCE_LIST CONFIGURE_DEPENDS "*.cpp")

source_group("include" FILES ${INCLUDE_LIST})
source_group("source" FILES ${SOURCE_LIST})

create_gallery_entry(
    TARGET vkx LIBRARY USE_PUBLIC
    SOURCES ${INCLUDE_LIST} ${SOURCE_LIST}
    ROOT ${PROJECT_ROOT}
    FOLDER "${ENTRY}"
    LIB
        Vulkan::Vulkan
        zeus::zeus
        glm::glm
        vk-bootstrap::vk-bootstrap
        VulkanMemoryAllocator
        glfw
    )

