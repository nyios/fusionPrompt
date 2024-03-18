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
    /// current position of the cursor (last line)
    unsigned cursorPosition;
    /// vertices array for the current letter that is uploaded to the GPU.
    /// Contains 2D positional coordinates for each of the four vertices 
    /// + 2D texture coordinates for each of the vertices
    std::array<float, 16> vertices;
    /// the shell that processes the input after the user has hit enter
    Shell shell;

    /*
     * render the current lines in input
     *
     * @param width Width of the characters on the current screen
     * @param height Height of the characters
     * @param sCharacters The shader program for the characters
     * @param sCursor The shader program for the cursor
     * @param colored The location of the color uniform to change the color of the text 
     */
    void renderString(float width, float height, Shader& sCharacters, Shader& sCursor, int colorLocation);

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

    /// character callback function that is called as soon as a unicode character is typed in (no control characters) !!Interprets the unicode as an 8 bit ascii, thats it!!
    friend void character_callback(GLFWwindow* window, unsigned int codepoint);
    /// key callback function that is called as soon as any key is pressed
    friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    /// scroll callback function that is called as soon as the user scrolls
    friend void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


};
