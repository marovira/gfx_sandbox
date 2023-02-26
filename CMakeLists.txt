#================================
# Project setup.
#================================
cmake_minimum_required(VERSION 3.24)
project(gfx_sandbox LANGUAGES CXX)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

#================================
# Directory variables.
#================================
set(GFX_SANDBOX_SOURCE_DIR ${PROJECT_SOURCE_DIR})
set(GFX_SANDBOX_EXTERNAL_ROOT ${GFX_SANDBOX_SOURCE_DIR}/external)
set(GFX_SANDBOX_CMAKE_ROOT ${GFX_SANDBOX_SOURCE_DIR}/cmake)
set(GFX_SANDBOX_GALLERY_ROOT ${GFX_SANDBOX_SOURCE_DIR}/gallery)

#================================
# Find Packages.
#================================
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${GFX_SANDBOX_CMAKE_ROOT})
find_package(Vulkan REQUIRED)
find_package(zeus REQUIRED)
find_package(glm 0.9.9.9 REQUIRED)
find_package(glfw3 3.3.6 REQUIRED)
find_package(assimp 5.2.4 REQUIRED)

# Vulkan-bootstrap and VMA don't have install logic, so we're going to pull them through
# fetch content.
include(FetchContent)

# Vk-bootstrap v0.6
FetchContent_Declare(
    vk_bootstrap
    GIT_REPOSITORY https://github.com/charles-lunarg/vk-bootstrap
    GIT_TAG aeed599098060a1ecf6a5e6266748681f7ccc358
    )

# VMA v3.0.1
FetchContent_Declare(
    vma
    GIT_REPOSITORY https://github.com/GPUOpen-LibrariesAndSDKs/VulkanMemoryAllocator
    GIT_TAG a6bfc237255a6bac1513f7c1ebde6d8aed6b5191
    )


FetchContent_MakeAvailable(vk_bootstrap)
FetchContent_MakeAvailable(vma)

set_target_properties(vk-bootstrap PROPERTIES FOLDER "external")
set_target_properties(VulkanMemoryAllocator PROPERTIES FOLDER "external")

include(gallery_utils)

#================================
# Add subdirectories.
#================================
add_subdirectory(${GFX_SANDBOX_EXTERNAL_ROOT})
add_subdirectory(${GFX_SANDBOX_GALLERY_ROOT})
