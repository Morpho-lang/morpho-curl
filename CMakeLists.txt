cmake_minimum_required(VERSION 3.14)

project(morpho-curl)

# Build the library as a plugin
add_library(curl MODULE "") 

# Suppress 'lib' prefix
set_target_properties(curl PROPERTIES PREFIX "")

# Add sources
add_subdirectory(src)

###

# Locate the morpho.h header file and store in MORPHO_HEADER
find_file(MORPHO_HEADER
          morpho.h 
          HINTS 
          /usr/local/include/morpho
          /usr/local/opt/morpho
          /opt/homebrew/opt/morpho
          /home/linuxbrew/.linuxbrew/include/morpho
          )

# Identify folder that morpho.h is located in from MORPHO_HEADER and store in MORPHO_INCLUDE
get_filename_component(MORPHO_INCLUDE ${MORPHO_HEADER} DIRECTORY)

# Add morpho headers to MORPHO_INCLUDE
target_include_directories(curl PUBLIC ${MORPHO_INCLUDE})

# Add general header search paths
target_include_directories(curl PUBLIC /usr/local/include /opt/homebrew/include /home/linuxbrew/.linuxbrew/include)

# Add morpho headers in subfolders to MORPHO_INCLUDE
file(GLOB morpho_subdirectories LIST_DIRECTORIES true ${MORPHO_INCLUDE}/*)
foreach(dir ${morpho_subdirectories})
    IF(IS_DIRECTORY ${dir})
        target_include_directories(curl PUBLIC ${dir})
    ELSE()
        CONTINUE()
    ENDIF()
endforeach()

# Locate libmorpho
find_library(MORPHO_LIBRARY
    NAMES morpho libmorpho
)

# Locate libcurl
find_package(CURL)
target_include_directories(curl PUBLIC CURL_INCLUDE_DIRS)

target_link_libraries(curl ${MORPHO_LIBRARY} ${CURL_LIBRARIES})

set(CMAKE_INSTALL_PREFIX ..)

# Install the resulting binary
install(TARGETS curl LIBRARY DESTINATION lib/)
