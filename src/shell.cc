#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>
#include <limits.h>
#include <iostream>
#include <filesystem>
#include "shell.h"

Shell::Shell() {
    // set host and user name
    char hostname[HOST_NAME_MAX];
    char username[LOGIN_NAME_MAX];
    gethostname(hostname, HOST_NAME_MAX);
    getlogin_r(username, LOGIN_NAME_MAX);
    this->host = hostname;
    this->user = username;
}

std::string Shell::getPreamble() {
    return this->user + "@" + this->host + ":" + std::filesystem::current_path().string() + "$ ";
}

std::vector<std::string> Shell::tokenize(const std::string& input) {
    std::string current;
    std::vector<std::string> res;
    bool inQuotation = false;
    for (auto &c : input) {
        switch(c) {
            case '"': 
                inQuotation = !inQuotation;
                break;
            case ' ': 
                if (inQuotation) {
                    current.push_back(c);
                } else if (!current.empty()) {
                    res.push_back(current);
                    current.clear();
                }
                break;
            case '\n':
                if (inQuotation) {
                    current.push_back(c);
                } else if (!current.empty()) {
                    res.push_back(current);
                    current.clear();
                }
                break;
            default: current.push_back(c);
        }
    }
    if (!current.empty())
        res.push_back(current);
    return res;
}

std::string Shell::getOutputString(const std::string& input){
    std::vector<std::string> tokenInput = tokenize(input);
    if (this->builtins.contains(tokenInput[0])) {
    // handle builtins
        return executeBuiltin(tokenInput);
    } else {
        return execute(tokenInput);
    }
}

std::string Shell::executeBuiltin(const std::vector<std::string>& command) {
    if (command[0] == "cd") {
        std::filesystem::current_path(command[1]); 
        return "";
    } else if (command[0] == "exit") {
        long ret = 0;
        if (command.size() > 1)
            ret = strtol(command[1].c_str(), NULL, 10);
        exit(ret);
    } else {
        return "sorry didn't implement that yet";
    }
}

std::string Shell::execute(std::vector<std::string>& command) {
    char* argv [command.size() + 1];
    for (unsigned i = 0; i < command.size(); ++i) {
        argv[i] = (char*) command[i].c_str();
    }
    // argv needs to be null terminated for execvp
    argv[command.size()] = NULL;
    pid_t pid;
    int fd[2];
    unsigned N = 4096;
    char inbuf[N];

    if (pipe(fd) < 0) {
        perror("pipe");
        exit(-1);
    }
    switch(pid = fork()) {
        case 0:
            //child process
            dup2(fd[1], STDOUT_FILENO); 
            dup2(fd[1], STDERR_FILENO); 
            close(fd[1]);
            execvp(command[0].c_str(), argv);
            // execvp should not return, if it does, an error occured
            perror("execvp");
            exit(1);
            break;
        case -1:
            //something went wrong
            perror("fork");
            exit(-1);
            break;
        default:
            //parent process
            int status;
            int n = 0;
            int i = 0;
            close(fd[1]);
            while ((i = read(fd[0], inbuf+n, N-1-n)) != 0) {
                if (i == -1) {
                    perror("read");
                    exit(1);
                }
                n += i;
            }
            inbuf[n] = '\0';
            close(fd[0]);
            waitpid(pid, &status, 0);
            break;
    }
    return std::string{inbuf};
}
