#pragma once
#include <string>
class Shell {
    std::string host;
    std::string user;
    std::string current_path;
    std::string output;

    public:
        Shell();
        std::string getOutputString(const std::string& input);
        std::string getPreamble();
        static std::string execute(const std::string& command);

};
