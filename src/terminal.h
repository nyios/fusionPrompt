#pragma once
#include <string>
#include <array>
#include <vector>
#include <glad/glad.h> 
#include <GLFW/glfw3.h>
#include "shader.h"
#include "shell.h"

/*
 * Terminal class that is responsible for rendering input and output to the screen
 */
class Terminal {
    /// width of the terminal window
    unsigned width;
    /// height of the terminal window
    unsigned height;
    /// what should currently be displayed in the window, linewise
    std::vector<std::string> input;
    /// line of input that is displayed first
    unsigned line = 0;
    /// vertices array for the current letter that is uploaded to the GPU.
    /// Contains 2D positional coordinates for each of the four vertices 
    /// + 2D texture coordinates for each of the vertices
    std::array<float, 16> vertices;
    /// the shell that processes the input after the user has hit enter
    Shell s;

    /*
     * render the current string in input
     *
     * @param width Width of the characters on the current screen
     * @param height Height of the characters
     */
    void renderString(float width, float height);

    /*
     * Binds the VAO and VBO, uploads the vertices to the GPU and loads the texture
     */
    void GPUSetup();

    public:
        /*
         * Terminal Constructor
         *
         * @param widthArg The width of the terminal window
         * @param heightArg The height of the terminal window
         */
        Terminal(unsigned widthArg, unsigned heightArg);

        /*
         * Starts the render loop
         */
        void start();

    /// character callback function that is called as soon as a utf8 character is typed in (no control characters)
    friend void character_callback(GLFWwindow* window, unsigned int codepoint);
    /// key callback function that is called as soon as any key is pressed
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


};
