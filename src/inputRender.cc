#include <iostream>
#include "inputRender.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

InputRender::InputRender(unsigned widthArg, unsigned heightArg) : 
    width{widthArg}, height{heightArg} {
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void character_callback(GLFWwindow* window, unsigned int codepoint) {
    reinterpret_cast<InputRender*>(glfwGetWindowUserPointer(window))->input.push_back((char) codepoint);
}

void InputRender::processInput(GLFWwindow *window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

//TODO make vertices an attribute
void InputRender::GPUSetup(float vertices[16]) {
    unsigned VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 16, vertices, GL_DYNAMIC_DRAW);

    // we want to render quads
    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute: starting at 0 with stride 4 (2 vertex coordinates, 2 texture coordinates) and offset 0
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
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

// TODO make vertices a std::array
void InputRender::renderString(float vertices[16]) {
    for (auto &c : input) {
        // set position attribute correctly
        vertices[0] += 0.025;
        vertices[4] += 0.025;
        vertices[8] += 0.025;
        vertices[12] += 0.025;
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
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(float) * 16, vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }
}

void InputRender::renderInput() {
    // 2 vertex coordinates, 2 texture coordinates
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
    //register callback function the user inputs a character
    glfwSetCharCallback(window, character_callback);
    //set render object as user pointer in the window, to access it in the callback function
    glfwSetWindowUserPointer(window, this);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    // build and compile our shader program
    Shader s("../shaderSrc/shader.vs", "../shaderSrc/shader.fs"); 

    glCullFace(GL_NONE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    glClearColor(0.0f, 0.05f, 0.2f, 1.0f);
    float vertices[] = {
        -1.0f,  1.0f, 0.1f, 0.0f,
        -1.0f,  0.93f, 0.1f, 0.1f,
        -1.025f,  0.93f, 0.0f, 0.1f,
        -1.025f,  1.0f, 0.0f, 0.0f
    };
    GPUSetup(vertices);

    //render loop
    while(!glfwWindowShouldClose(window)) {
        //reset vertices to start in upper left corner
        float vertices[] = {
            -1.0f,  1.0f, 0.1f, 0.0f,
            -1.0f,  0.93f, 0.1f, 0.1f,
            -1.025f,  0.93f, 0.0f, 0.1f,
            -1.025f,  1.0f, 0.0f, 0.0f
        };
        processInput(window);
        glClear(GL_COLOR_BUFFER_BIT);
        renderString(vertices);
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }
    std::cout << this->input << std::endl;
    glfwTerminate();
}
