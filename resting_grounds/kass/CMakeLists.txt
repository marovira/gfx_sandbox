set(KASS_ROOT ${CMAKE_CURRENT_LIST_DIR})

set(LIBKASS_INCLUDE_LIST
    ${KASS_ROOT}/konvert_image.hpp
    ${KASS_ROOT}/konverter.hpp
    )

set(LIBKASS_SOURCE_LIST
    ${KASS_ROOT}/konvert_image.cpp
    ${KASS_ROOT}/konverter.cpp
    )

source_group("include" FILES ${LIBKASS_INCLUDE_LIST})
source_group("source" FILES ${LIBKASS_SOURCE_LIST})

add_library(libkass ${LIBKASS_SOURCE_LIST} ${LIBKASS_INCLUDE_LIST})
target_link_libraries(libkass PRIVATE
    core 
    assets
    assimp::assimp
    stb
    glm::glm
    )

if (VK_VIEWER_USE_NVTT)
    target_link_libraries(libkass PRIVATE nvtt::nvtt)
    target_compile_definitions(libkass PRIVATE -DKASS_USE_NVTT)
endif()

if (VK_VIEWER_BUILD_KASS)
    set(SOURCE_LIST
        ${KASS_ROOT}/kass.cpp
        )

    source_group("source" FILES ${SOURCE_LIST})

    add_executable(kass ${SOURCE_LIST})
    target_link_libraries(kass PRIVATE 
        libkass
        argparse::argparse
        core
        )

    if (VK_VIEWER_USE_NVTT)
        # Make sure the NVTT DLL is copied over.
        if (WIN32)
            add_custom_command(TARGET kass POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy ${NVTT_SHARED_LIBS}
                $<TARGET_FILE_DIR:kass>
                )
        endif()
    endif()
endif()
