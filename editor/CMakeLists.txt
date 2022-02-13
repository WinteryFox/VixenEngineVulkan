project(editor)

add_executable(editor main.cpp)
target_include_directories(editor PUBLIC ../engine/src)
