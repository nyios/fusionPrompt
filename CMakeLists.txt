cmake_minimum_required(VERSION 3.25)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
project(fusionPrompt)
add_executable(fusionPrompt
    src/main.cc
    src/glad.c
    src/shader.cc
    src/terminal.cc
    src/shell.cc)
target_compile_features(fusionPrompt PUBLIC cxx_std_20)
target_link_libraries(fusionPrompt
    glfw
    )
