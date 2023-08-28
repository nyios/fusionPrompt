#pragma once
#include <string>

/**
 * Shader class that compiles both the vertex and fragment shader given by their file path
 * and links them into a program
 */
class Shader {
    static void checkCompileErrors(unsigned int shader, const std::string& type);
    public:
        Shader(const std::string& vertexShader, const std::string& fragmentShader);
};
