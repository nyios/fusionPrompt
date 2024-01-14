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

std::string Shell::getOutputString(const std::string& input){
    if (input.starts_with("cd ")) {
        std::filesystem::current_path(input.substr(3)); 
        return "";
    } else {
        return execute(input);
    }
}

std::string Shell::execute(const std::string& command) {
    auto exe = command.substr(0, command.find(" "));
    auto arg = command.substr(command.find(" ") + 1);
   // std::cout << "command: " << exe << " with arguments: " << arg << " called\n";
    pid_t pid;
    int fd[2];
    char inbuf[4096];

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
            close(fd[0]);
            execlp(exe.c_str(), exe.c_str(), arg.c_str(), (char*) NULL);
            perror("execlp");
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
            int n;
            close(fd[1]);
            while(1) {
                n = read(fd[0], inbuf, 1024);
                if (n == -1) {
                    perror("read");
                    exit(1);
                } else if (n == 0) {
                    break;
                }
            }
            close(fd[0]);
            waitpid(pid, &status, 0);
            break;
    }
    return std::string{inbuf};
}
