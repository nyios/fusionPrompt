#pragma once
#include <string>

class Shader {
    int program;
    void checkCompileErrors(unsigned int shader, const std::string& type);
    public:
        Shader(const std::string& vertexShader, const std::string& fragmentShader);
        int getID();

};
