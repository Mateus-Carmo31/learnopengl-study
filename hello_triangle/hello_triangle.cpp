#include <iostream>
#include <glad/glad.h> // GLAD comes first!
#include <GLFW/glfw3.h>

void framebuffer_resize_callback(GLFWwindow* window, int w, int h);
void processInput(GLFWwindow* window);

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(0.0f, 0.5f, 0.2f, 1.0f);\n"
    "}\0";


int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello Triangle!", NULL, NULL);
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

    /*
    ** ============== SHADERS =============
     */

    // Setting up the vertex shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);

    // Attaching the source code and compiling
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // Error checking
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Setting up the fragment shader
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Creating the Shader Program Object
    // This is basically like linking object files, the final program will
    // have the compiled shaders all wired up properly (outputs connected to inputs)
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    // Shaders aren't necessary after the program has been compiled in runtime.
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*
    ** ============== VBOS/VAOS =============
     */

    // Triangle coordinates in NDC space (ranges from -1 to 1)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    // Generating Vertex Array Buffer
    // This will store our vertex buffer(s) and vertex attribute configurations
    unsigned int VAO;
    glGenVertexArrays(1,&VAO);

    // Generating Vertex Buffer Object.
    // This allows us to make a buffer in the GPU for our vertices.
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Binding the VAO
    // Now subsequent vertex buffer/attribute modifications will be registered in the VAO
    glBindVertexArray(VAO);

    // Binding the VBO
    // OpenGL has multiple "targets" where data can be put to be modified
    // by its functions. Data is "bound" to targets. VBOs are bound to the
    // GL_ARRAY_BUFFER target, and only one can be bound to a target at a
    // time. We can bind as many buffers as we want, as long as it's to
    // individual (and appropriate) targets.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Sending the array data to the VBO
    // Note that we only mention the target, since the VBO is already bound.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

   // Now we instruct OpenGL how our vertex data (the vertex attributes) is laid out in memory.
   // Each vertex position is 3 floats of 4 bytes each, densely packed, starting at the beginning of the buffer.
   // Args:
   //  - Location of the attribute ( location = 0 )
   //  - Size (vec3)
   //  - Type of data
   //  - Should we normalize? (fit to range 0-1)
   //  - Stride (how many bytes between vertex attributes in the array, which is 3 * float)
   //  - Offset where the data begins in the buffer.
   glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*) 0);
   glEnableVertexAttribArray(0);

   // Note: this won't set the VAO's buffer to 0, so it's safe to do.
   glBindBuffer(GL_ARRAY_BUFFER, 0);

   // Unbinding the VAO isn't always necessary since binding another would already unbind this one.
   glBindVertexArray(0);

    while(!glfwWindowShouldClose(window))
    {
        processInput(window);

        // Rendering stuff here
        glClearColor(0.0f, 0.05f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void framebuffer_resize_callback(GLFWwindow* window, int w, int h)
{
    glViewport(0, 0, w, h);
}

void processInput(GLFWwindow* window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}
