#include <iostream>
#include <unistd.h>
#include <sstream>
#include "terminal.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

Terminal::Terminal(unsigned widthArg, unsigned heightArg) : 
    width{widthArg}, height{heightArg} {
            input.push_back(s.getPreamble());
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
    auto renderer = reinterpret_cast<Terminal*>(glfwGetWindowUserPointer(window));
    renderer->input.back().push_back((char) codepoint);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    auto renderer = reinterpret_cast<Terminal*>(glfwGetWindowUserPointer(window));
    //enter key has been pressed, process input and display result
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        //get command (behind preamble)
        auto command = renderer->input.back().substr(renderer->s.getPreamble().size());
        //append result to input that needs to be displayed
        std::stringstream ss(renderer->s.getOutputString(command));
        std::string token;
        while (std::getline(ss, token, '\n')) {
            renderer->input.push_back(token);
        }   
        //start new line with preamble
        renderer->input.push_back(renderer->s.getPreamble());
        if (renderer->input.size() > 30)
            renderer->line = renderer->input.size() - 30;
    // <esc> exists window
    } else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    // backspace deletes the last character
    } else if (key == GLFW_KEY_BACKSPACE && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        if (!(renderer->input.back() == renderer->s.getPreamble()))
            renderer->input.back().pop_back();
    }
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    auto renderer = reinterpret_cast<Terminal*>(glfwGetWindowUserPointer(window));
    if (renderer->input.size() > 30) {
        int newFirstLine = renderer->line - yoffset;
        if (newFirstLine >= 0 && newFirstLine < renderer->input.size())
            renderer->line = newFirstLine;
    }
}


void Terminal::GPUSetup() {
    unsigned VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, vertices.data(), GL_DYNAMIC_DRAW);

    // we want to render quads
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute: starting at 0 with stride 4 (2 vertex coordinates, 2 texture coordinates) and offset 0
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // texture coord attribute
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    // load and create a texture 
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    unsigned char *data = stbi_load("../textures/font.png", &width, &height, &nrChannels, 4);
    std::cout << "loaded texture " << nrChannels << std::endl;
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        std::cout << "bound texture\n";
    }
    else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}

void Terminal::renderString(float character_width, float character_height) {
    for (unsigned i = this->line; i < this->input.size(); ++i) {
        auto line = this->input[i];
        for (auto &c : line) {
            // set position attribute correctly (only x direction)
            vertices[0] += character_width;
            vertices[4] += character_width;
            vertices[8] += character_width;
            vertices[12] += character_width;
            // space character, nothing to draw
            if (c == 32)
                continue;
            // set texture attribute correctly
            uint8_t pos_x = (c - 33) % 10;
            uint8_t pos_y = (c - 33) / 10;
            vertices[2] = 0.1f + pos_x / 10.0f;
            vertices[3] = pos_y / 10.0f;
            vertices[6] = 0.1f + pos_x / 10.0f;
            vertices[7] = 0.1f + pos_y / 10.0f;
            vertices[10] = pos_x / 10.0f;
            vertices[11] = 0.1f + pos_y / 10.0f;
            vertices[14] = pos_x / 10.0f;
            vertices[15] = pos_y / 10.0f;
            glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, vertices.data());
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
        // only do the new line if something is coming afterwards
        if (i != this->input.size() - 1) {
            //new line, start at beginning, one lower
            vertices[0] = -1.0f;
            vertices[4] = -1.0f;
            vertices[8] = -1.0f - character_width;
            vertices[12] = -1.0f - character_width;
            vertices[1] -= character_height;
            vertices[5] -= character_height;
            vertices[9] -= character_height;
            vertices[13] -= character_height;
        }
    }
}

void Terminal::start() {
    glfwInit();
    //setup glfw context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //create window object with specified width, height and name
    GLFWwindow* window = glfwCreateWindow(this->width, this->height, "Shell", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);
    //register callback function when window is resized
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //register callback function that is called when the user inputs a character
    glfwSetCharCallback(window, character_callback);
    //register callback function that is called when the user inputs some control character
    glfwSetKeyCallback(window, key_callback);
    //register callback function that is called when the user scrolls
    glfwSetScrollCallback(window, scroll_callback);
    //set render object as user pointer in the window, to access it in the callback function
    glfwSetWindowUserPointer(window, this);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    // build and compile our shader programs
    Shader sCharacters("../shaderSrc/shader.vs", "../shaderSrc/shaderCharacters.fs"); 
    // the cursor doesnt need a texture, it is a simple colored quad
    Shader sCursor("../shaderSrc/shader.vs", "../shaderSrc/shaderCursor.fs"); 

    glCullFace(GL_NONE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
    GPUSetup();

    //render loop
    while(!glfwWindowShouldClose(window)) {
        int width, height;
        glfwGetFramebufferSize(window, &width, &height);
        float character_width = 28.0f / width;
        float character_height = 66.0f / height;
        //reset vertices to start in upper left corner
        this->vertices = {
            -1.0f,  1.0f, 0.1f, 0.0f,
            -1.0f,  1.0f - character_height, 0.1f, 0.1f,
            -1.0f - character_width,  1.0f - character_height, 0.0f, 0.1f,
            -1.0f - character_width,  1.0f, 0.0f, 0.0f
        };
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(sCharacters.getShaderProgram());
        renderString(character_width, character_height);
        // render cursor
        glUseProgram(sCursor.getShaderProgram());
        vertices[0] += character_width; // set quad one more to the right
        vertices[4] += character_width;
        vertices[8] += character_width;
        vertices[12] += character_width;
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, vertices.data());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    glfwTerminate();
}
