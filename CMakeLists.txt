cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Shell)
set(CMAKE_CXX_STANDARD 17)
add_executable(Shell
    src/main.cc
    src/glad.c)
target_link_libraries(Shell
    glfw
    )
