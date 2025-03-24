project(kernel)
cmake_minimum_required(VERSION 3.15)
message(STATUS "CMAKE_SYSTEM: " ${CMAKE_SYSTEM})
message(STATUS "CMAKE_SYSTEM_NAME: " ${CMAKE_SYSTEM_NAME})
message(STATUS "CMAKE_SYSTEM_VERSION: " ${CMAKE_SYSTEM_VERSION})
message(STATUS "CMAKE_SYSTEM_PROCESSOR: " ${CMAKE_SYSTEM_PROCESSOR})

set(CMAKE_MODULE_PATH ${CMAKE_HOME_DIRECTORY}/cmake)
include(QfusionUtils)
include(OutputDirectory)
set(CMAKE_CXX_STANDARD 17)

set(CMAKE_SKIP_BUILD_RPATH FALSE)
 
# when building, don't use the install RPATH already
# (but later on when installing)
set(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
set(CMAKE_INSTALL_RPATH $ORIGIN/libs)
set(CMAKE_BUILD_RPATH $ORIGIN/libs:$ORIGIN)

get_filename_component(HE_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" ABSOLUTE)

OPTION(USE_SYSTEM_ZLIB "Prefer system cURL instead of the bundled one" OFF)
option(USE_GRAPHICS_X11 "Build using X11" ON)
option(USE_GRAPHICS_WAYLAND "Build using wayland" ON)
option(BUILD_UNIT_TEST "Build with unit test" OFF)

add_subdirectory(extern)
add_subdirectory(src/he)

