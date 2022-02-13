cmake_minimum_required(VERSION 3.6)
project(VixenEngine)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
if (WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static-libstdc++ -static-libgcc")
endif ()

if (POLICY CMP0025)
    cmake_policy(SET CMP0025 NEW)
endif ()

set(SKIP_INSTALL_ALL ON)

add_subdirectory(engine)
add_subdirectory(editor)
