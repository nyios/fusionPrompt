#pragma once
#include <string>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "shader.h"

class InputRender {
    unsigned width;
    unsigned height;
    std::string input;

    void renderString(float vertices[16]);
    void GPUSetup(float vertices[16]);
    void processInput(GLFWwindow *window);

    public:
        InputRender(unsigned widthArg, unsigned heightArg);
        void renderInput();
    friend void character_callback(GLFWwindow* window, unsigned int codepoint);
};
