cmake_minimum_required(VERSION 3.13)
project(VixenEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif (WIN32)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SKIP_INSTALL_ALL ON)

# ---- OPTIONS ----

if (CMAKE_BUILD_TYPE MATCHES "Debug")
    option(VIXEN_DEBUG "Debug output" ON)
else ()
    option(VIXEN_DEBUG "Debug output" OFF)
endif ()

set(VIXEN_DEBUG_LEVEL "VERBOSE" CACHE STRING "")
set_property(CACHE VIXEN_DEBUG_LEVEL PROPERTY STRINGS "VERBOSE;INFO;WARNING;ERROR")

option(VIXEN_BUILD_TEST "Build the test executable" ON)

# ---- BUILD ENGINE ----

add_subdirectory(engine/)

# ---- BUILD TEST ----

if (VIXEN_BUILD_TEST)
    add_subdirectory(test/)
endif (VIXEN_BUILD_TEST)