#-------------------------------------------------------------------------------
# morpho-curl/CMakeLists.txt
#-------------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.13)

# Default to a Release build when no build type is set.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "CMAKE_BUILD_TYPE unset; defaulting to Release")
    set(CMAKE_BUILD_TYPE Release CACHE STRING
        "Build type (Debug, Release, RelWithDebInfo, MinSizeRel)" FORCE)
    set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
        "Debug" "Release" "RelWithDebInfo" "MinSizeRel")
endif()

project(morpho-curl LANGUAGES C)

# Build the library as a plugin.
add_library(curl MODULE "")

# Suppress the 'lib' prefix.
set_target_properties(curl PROPERTIES PREFIX "")

#-------------------------------------------------------------------------------
# Release LTO
#-------------------------------------------------------------------------------

include(CheckIPOSupported)

check_ipo_supported(
    RESULT CURL_IPO_SUPPORTED
    OUTPUT CURL_IPO_OUTPUT
    LANGUAGES C
)

if(CURL_IPO_SUPPORTED)
    set_property(TARGET curl PROPERTY
        INTERPROCEDURAL_OPTIMIZATION_RELEASE TRUE)
    message(STATUS "IPO/LTO enabled for Release")
else()
    message(STATUS "IPO/LTO not enabled (${CURL_IPO_OUTPUT})")
endif()

#-------------------------------------------------------------------------------
# Sources
#-------------------------------------------------------------------------------

add_subdirectory(src)

#-------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------

find_package(morpho CONFIG REQUIRED)
find_package(CURL REQUIRED)

#-------------------------------------------------------------------------------
# Linking
#-------------------------------------------------------------------------------

target_link_libraries(curl PRIVATE morpho::morpho CURL::libcurl)

#-------------------------------------------------------------------------------
# Installation
#-------------------------------------------------------------------------------

if(CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}"
        CACHE PATH "Install prefix" FORCE)
endif()

install(TARGETS curl
    LIBRARY DESTINATION lib
)

# Install DLLs that sit beside the plugin.
if(WIN32)
    install(DIRECTORY "$<TARGET_FILE_DIR:curl>/"
        DESTINATION lib
        FILES_MATCHING PATTERN "*.dll"
    )
endif()
