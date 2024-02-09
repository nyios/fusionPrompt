#pragma once
#include <string>
#include <unordered_set>
#include <vector>

class Shell {
    std::string host;
    std::string user;
    std::string current_path;
    std::string output;
    std::unordered_set<std::string> builtins = {"cd", "exit", "export", "pwd", "alias"};
    static std::string execute(std::vector<std::string>& command);
    static std::string executeBuiltin(const std::vector<std::string>& command);
    static std::vector<std::string> tokenize(const std::string& command);


    public:
        Shell();
        std::string getOutputString(const std::string& input);
        std::string getPreamble();

};
