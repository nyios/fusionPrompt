cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(Shell)
add_executable(Shell
    src/main.cc
    src/glad.c
    src/shader.cc
    src/outputRender.cc
    src/parser.cc)
target_compile_features(Shell PUBLIC cxx_std_20)
target_link_libraries(Shell
    glfw
    )
