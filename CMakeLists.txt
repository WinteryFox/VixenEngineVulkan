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

# ---- BUILD ENGINE ----

add_subdirectory(engine/)

# ---- BUILD TEST ----

if (VIXEN_BUILD_TEST)
    add_subdirectory(test/)
endif (VIXEN_BUILD_TEST)