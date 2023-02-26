# Locate NVTT library
# This module defines
#  NVTT_INCLUDE_DIR, where to find nvtt.h
#  NVTT_SHARED_LIBS
#  NVTT_LIBRARIES, defined only in Windows builds
#  NVTT_VERSION_STRING, the version of NVTT found.
#=============================================================================
# The required version of NVTT can be specified using the 
# standard syntax, e.g. FIND_PACKAGE(nvtt 3.1.6). Note that
# major, minor, and patch versions must be provided, otherwise
# the module will search for any available implementation.
# Note: it is your responsibility to ensure that the shared library
# defined in NVTT_SHARED_LIBS is copied to the final executable.

set(_POSSIBLE_NVTT_INCLUDE include)

# Determine possible naming suffixes based on version.
if (nvtt_FIND_VERSION_MAJOR AND nvtt_FIND_VERSION_MINOR AND nvtt_FIND_VERSION_PATCH)
    # According to the header file, the version of NVTT is determined by the following
    # formula: 10000 * <major> + 100 * <minor> + maintenance.
    math(EXPR _REQUESTED_VERSION
        "10000 * ${nvtt_FIND_VERSION_MAJOR} + 100 * ${nvtt_FIND_VERSION_MINOR} + ${nvtt_FIND_VERSION_PATCH}"
        )
    set(_POSSIBLE_SUFFIXES "${_REQUESTED_VERSION}")
else()
    set(_POSSIBLE_SUFFIXES 
        "30106"
        )
endif()

# Set the possibilities for the shared libraries.
set(_POSSIBLE_NVTT_SHARED_LIBS)
foreach(_SUFFIX ${_POSSIBLE_SUFFIXES})
    list(APPEND _POSSIBLE_NVTT_SHARED_LIBS 
        "nvtt${_SUFFIX}.dll"
        "libnvtt.so.${_SUFFIX}"
        )
endforeach()

if (WIN32)
    set(_POSSIBLE_PATHS
        "C:/Program Files/NVIDIA Corporation/NVIDIA Texture Tools"
        )
else()
    # These may be incorrect, but I have no clue where NVTT gets installed under Linux.
    set(_POSSIBLE_PATHS
        "/usr/local"
        "/usr"
        "/sw"
        "/opt/local"
        "/opt/csw"
        "/opt"
        )
endif()

# Find the header
find_path(NVTT_INCLUDE_DIR nvtt/nvtt.h
    HINTS $ENV{NVTT_DIR}
    PATH_SUFFIXES ${_POSSIBLE_NVTT_INCLUDE}
    PATHS ${_POSSIBLE_PATHS}
    )

# Find the shared library.
find_file(NVTT_SHARED_LIBS
    NAMES ${_POSSIBLE_NVTT_SHARED_LIBS}
    HINTS $ENV{NVTT_DIR}
    PATHS ${_POSSIBLE_PATHS}
    )

# For Windows only, find the static library.
if (WIN32)
    set(_POSSIBLE_NVTT_LIB_PATHS
        "lib"
        "lib/x64-v142"
        )

    set(_POSSIBLE_NVTT_LIB)
    foreach(_SUFFIX ${_POSSIBLE_SUFFIXES})
        list(APPEND _POSSIBLE_NVTT_LIB
            "nvtt${_SUFFIX}.lib"
            )
    endforeach()

    find_file(NVTT_LIBRARIES
        NAMES ${_POSSIBLE_NVTT_LIB}
        PATH_SUFFIXES ${_POSSIBLE_NVTT_LIB_PATHS}
        PATHS ${_POSSIBLE_PATHS}
        )

    # TODO: Under Windows, NVTT requires the existence of the cudart DLL. It may be a good
    # idea to figure out whether we can grab it here as well and either append it to
    # NVTT_SHARED_LIBS or into a separate variable so it can be copied over.
endif()

# Grab the version string
if (NVTT_INCLUDE_DIR AND EXISTS "${NVTT_INCLUDE_DIR}/nvtt/nvtt.h")
    file(STRINGS "${NVTT_INCLUDE_DIR}/nvtt/nvtt.h" nvtt_version_str 
        REGEX "^#define[ \t]+NVTT_VERSION[ \t]+.+")
    string(REGEX REPLACE "^#define[ \t]+NVTT_VERSION[ \t]+([0-9]+)"
        "\\1" NVTT_VERSION_STRING "${nvtt_version_str}"
        )
endif()

include(FindPackageHandleStandardArgs)
include(SelectLibraryConfigurations)

if (WIN32)
    find_package_handle_standard_args(nvtt
        REQUIRED_VARS NVTT_INCLUDE_DIR NVTT_SHARED_LIBS NVTT_LIBRARIES
        VERSION_VAR NVTT_VERSION_STRING
        )
else()
    find_package_handle_standard_args(nvtt
        REQUIRED_VARS NVTT_INCLUDE_DIR NVTT_SHARED_LIBS
        VERSION_VAR NVTT_VERSION_STRING
        )
endif()

mark_as_advanced(NVTT_INCLUDE_DIR NVTT_LIBRARIES NVTT_SHARED_LIBS)

if (NVTT_FOUND AND NOT TARGET nvtt::nvtt_lib)
    add_library(nvtt::nvtt STATIC IMPORTED)
    if (WIN32)
        set_target_properties(nvtt::nvtt PROPERTIES
            IMPORTED_LOCATION ${NVTT_LIBRARIES}
            INTERFACE_INCLUDE_DIRECTORIES ${NVTT_INCLUDE_DIR}
            )
    else()
        set_target_properties(nvtt::nvtt PROPERTIES
            INTERFACE_INCLUDE_DIRECTORIES ${NVTT_INCLUDE_DIR}
            )
    endif()
endif()
