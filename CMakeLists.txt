cmake_minimum_required(VERSION 3.13)
project(VixenEngine)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (WIN32)
    set(CMAKE_CXX_STANDARD_LIBRARIES "-static -static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mthreads -Wl,--allow-multiple-definition -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
endif (WIN32)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(SKIP_INSTALL_ALL ON)

# ---- OPTIONS ----

option(VIXEN_BUILD_TEST "Build the test executable" ON)
option(VIXEN_WITH_GLFW "Build the engine with GLFW" ON)

set(VIXEN_LOGGER_MODE "VIXEN_TRACE" CACHE STRING "The desired logger mode")
set_property(CACHE VIXEN_LOGGER_MODE PROPERTY STRINGS "VIXEN_FATAL" "VIXEN_ERROR" "VIXEN_WARNING" "VIXEN_INFO" "VIXEN_TRACE" "VIXEN_NONE")

if (VIXEN_LOGGER_MODE STREQUAL "VIXEN_NONE")
    add_compile_definitions(VIXEN_DEBUG_LEVEL=0)
elseif (VIXEN_LOGGER_MODE STREQUAL "VIXEN_FATAL")
    add_compile_definitions(VIXEN_DEBUG_LEVEL=1)
elseif (VIXEN_LOGGER_MODE STREQUAL "VIXEN_ERROR")
    add_compile_definitions(VIXEN_DEBUG_LEVEL=2)
elseif (VIXEN_LOGGER_MODE STREQUAL "VIXEN_WARNING")
    add_compile_definitions(VIXEN_DEBUG_LEVEL=3)
elseif (VIXEN_LOGGER_MODE STREQUAL "VIXEN_INFO")
    add_compile_definitions(VIXEN_DEBUG_LEVEL=4)
elseif (VIXEN_LOGGER_MODE STREQUAL "VIXEN_TRACE")
    add_compile_definitions(VIXEN_DEBUG_LEVEL=5)
endif (VIXEN_LOGGER_MODE STREQUAL "VIXEN_NONE")

# ---- BUILD ENGINE ----

add_subdirectory(engine/)

# ---- BUILD TEST ----

if (VIXEN_BUILD_TEST)
    add_subdirectory(test/)
endif (VIXEN_BUILD_TEST)