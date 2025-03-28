
if(USE_SYSTEM_ZLIB)
  find_package(ZLIB REQUIRED GLOBAL)
  add_library(zlib ALIAS ZLIB::ZLIB)
  target_link_libraries(ZLIB::ZLIB INTERFACE ZLIB::ZLIB)
else()
  option(ZLIB_BUILD_EXAMPLES "" OFF)
  add_subdirectory(zlib)
endif()


if(CMAKE_SYSTEM_NAME STREQUAL Linux)
    if (USE_GRAPHICS_X11)
        find_path (XLIB_HEADERS NAMES "X11/Xlib.h")
        if (NOT XLIB_HEADERS)
            message (FATAL_ERROR "Can't find 'X11/xlib.h'.(libx11-dev is not installed?)")
        endif ()
        list(APPEND VOLK_STATIC_DEFINES  VK_USE_PLATFORM_XLIB_KHR)
    endif ()
    if (USE_GRAPHICS_WAYLAND)
        find_path (WAYLAND_HEADERS NAMES "wayland-client.h")
        if (NOT WAYLAND_HEADERS)
            message (FATAL_ERROR "Can't find 'wayland-client.h'.(libwayland-dev is not installed?)")
        endif ()
        list(APPEND VOLK_STATIC_DEFINES VK_USE_PLATFORM_WAYLAND_KHR)
    endif ()
elseif (${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  list(APPEND VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
endif()

set(VULKAN_HEADERS_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Vulkan-Headers" )
option(VOLK_INSTALL "" OFF)
add_subdirectory(volk) 

add_library(utest INTERFACE)
target_include_directories(utest SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/utest")

add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/stb")

