#include "../wrappers/shader.hpp"
#include <iostream>
#include <cmath>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

void framebuffer_resize_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window, float *mix, int *mult);

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Textures", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

    Shader shader { "vertexShader.glsl", "fragShader.glsl" };

    // Vertices using EBO (so we need to specify the indices)
    float vertices[] = {
        // positions          // colors           // texture coords
        0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f,   // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,   // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f    // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    GLuint VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Note that we now have another vertex attribute, so we have to consider it in the stride
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // ==============
    // LOADING IMAGES
    // ==============

    // Images are normally top-down ((0,0) at the top left of the screen),
    // while OpenGL expects (0,0) to be at the bottom left. So we flip the image.
    stbi_set_flip_vertically_on_load(true);

    // Generating the OpenGL texture
    GLuint textures[2];
    glGenTextures(2, textures);

    // Just like other objects, we bind them so that we can modify them.
    // In this case, we bind it to the GL_TEXTURE_2D slot.
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    // Adjust parameters like wrapping and filtering
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Using stb_image to load the jpg.
    int width, height, nChannels;
    unsigned char *data = stbi_load("container.jpg", &width, &height, &nChannels, 0);

    if (data)
    {
        // This pumps the image data into the GPU.
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture!" << std::endl;
    }

    // We can free it now, since we loaded the data.
    stbi_image_free(data);

    // Now for the second texture.
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("awesomeface.png", &width, &height, &nChannels, 0);

    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cerr << "Failed to load texture!" << std::endl;
    }
    stbi_image_free(data);

    int mult = 1.0;
    float mix = 1.0f;

    shader.use();
    shader.setUniform("texture1", 0);
    shader.setUniform("texture2", 1);
    shader.setUniform("mult_amount", mult);
    shader.setUniform("mix_amount", mix);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window, &mix, &mult);

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Select the texture unit first.
        // This tells OpenGL where each texture goes, allowing us to bind multiple textures in multiple slots.
        // Each unit ID is adjacent, so we can do stuff like GL_TEXTURE0 + 1
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);

        // Drawing 6 indices aka elements
        shader.use();

        shader.setUniform("mult_amount", mult);
        shader.setUniform("mix_amount", mix);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window, float *mix, int *mult)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // I should probably use key callbacks instead!
    if(glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        *mix += 0.1;
        if (*mix > 1.0)
            *mix = 1.0;
    }

    if(glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        *mix -= 0.1;
        if (*mix < 0.0)
            *mix = 0.0;
    }

    if(glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        *mult += 1;

    if(glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        *mult -= 1;
        if (*mult < 1)
            *mult = 1;
    }
}
