#pragma once
#include <string>
#include <array>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "shader.h"
#include "parser.h"

class OutputRender {
    unsigned width;
    unsigned height;
    std::string preamble;
    std::string input;
    std::array<float, 16> vertices;
    Parser p;

    void renderString(int width, int height);
    void GPUSetup();

    public:
        OutputRender(unsigned widthArg, unsigned heightArg);
        void renderInput();
    friend void character_callback(GLFWwindow* window, unsigned int codepoint);
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

};
