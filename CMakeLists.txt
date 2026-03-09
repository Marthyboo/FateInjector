
cmake_minimum_required(VERSION 3.20)
project(fateInjector VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)

set(wxBUILD_SHARED OFF CACHE BOOL "OFF for static library!" FORCE) # force wxWidgets to use static library
mark_as_advanced(wxBUILD_SHARED)

set(wxUSE_WEBVIEW OFF CACHE BOOL "" FORCE)
set(wxUSE_LIBPNG OFF CACHE BOOL "" FORCE)
set(wxUSE_LIBTIFF OFF CACHE BOOL "" FORCE)
set(wxUSE_LIBWEBP OFF CACHE BOOL "" FORCE)
set(wxUSE_LIBJPEG OFF CACHE BOOL "" FORCE)
set(wxUSE_NANOSVG OFF CACHE BOOL "" FORCE)
set(wxUSE_LUNASVG OFF CACHE BOOL "" FORCE)
set(wxUSE_ARTPROVIDER_TANGO OFF CACHE BOOL "" FORCE)
set(wxUSE_SVG OFF CACHE BOOL "" FORCE)

add_executable(fateInjector WIN32)
add_subdirectory(src)

SET_TARGET_PROPERTIES(fateInjector PROPERTIES LINK_FLAGS "/level='requireAdministrator' /uiAccess='false'") # requireAdministrator

add_subdirectory(wxWidgets-3.3.2)

target_link_libraries(fateInjector wx::net wx::core wx::base)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})

