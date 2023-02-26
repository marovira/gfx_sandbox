set(LIB_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(INCLUDE_LIST
    ${LIB_ROOT}/asset_file.hpp
    ${LIB_ROOT}/material_asset.hpp
    ${LIB_ROOT}/mesh_asset.hpp
    ${LIB_ROOT}/prefab_asset.hpp
    ${LIB_ROOT}/texture_asset.hpp
    ${LIB_ROOT}/types.hpp
    )

set(SOURCE_LIST
    ${LIB_ROOT}/asset_file.cpp
    ${LIB_ROOT}/material_asset.cpp
    ${LIB_ROOT}/mesh_asset.cpp
    ${LIB_ROOT}/prefab_asset.cpp
    ${LIB_ROOT}/texture_asset.cpp
    )

source_group("source" FILES ${SOURCE_LIST})
source_group("include" FILES ${INCLUDE_LIST})

add_library(assets ${SOURCE_LIST} ${INCLUDE_LIST})
target_include_directories(assets PUBLIC ${VK_VIEWER_SOURCE_ROOT})
target_link_libraries(assets PUBLIC core)
target_link_libraries(assets PRIVATE 
    nlohmann_json::nlohmann_json
    lz4
    )
