#include "shader.hpp"
#include <fstream>
#include <sstream>
#include <iostream>

Shader::Shader(const char* vShaderPath, const char* fShaderPath)
{
    std::string vertexCode;
    std::string fragCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;

    // Setting exceptions
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

    try
    {
        vShaderFile.open(vShaderPath);
        fShaderFile.open(fShaderPath);

        std::stringstream vStream, fStream;
        vStream << vShaderFile.rdbuf();
        fStream << fShaderFile.rdbuf();

        vShaderFile.close();
        fShaderFile.close();

        vertexCode = vStream.str();
        fragCode   = fStream.str();
    }
    catch (std::ifstream::failure e)
    {
        std::cout << "ERROR::SHADER::FILE::FAILED_FILE_READ\nAre the shader files accessible?" << std::endl;
    }

    GLuint vertex, frag;
    int success;
    char infoLog[512];

    vertex = glCreateShader(GL_VERTEX_SHADER);
    const char* vCode = vertexCode.data();
    glShaderSource(vertex, 1, &vCode, NULL);
    glCompileShader(vertex);
    glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertex, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    frag = glCreateShader(GL_FRAGMENT_SHADER);
    const char* fCode = fragCode.data();
    glShaderSource(frag, 1, &fCode, NULL);
    glCompileShader(frag);
    glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(frag, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAG::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    _handle = glCreateProgram();
    glAttachShader(_handle, vertex);
    glAttachShader(_handle, frag);
    glLinkProgram(_handle);
    glGetProgramiv(_handle, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(_handle, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::LINK::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    glDeleteShader(vertex);
    glDeleteShader(frag);
}


void Shader::setUniform(const std::string &name, bool value) const
{
    glUniform1i(glGetUniformLocation(_handle, name.data()), (int) value);
}

void Shader::setUniform(const std::string &name, int value) const
{
    glUniform1i(glGetUniformLocation(_handle, name.data()), value);
}

void Shader::setUniform(const std::string &name, float value) const
{
    glUniform1f(glGetUniformLocation(_handle, name.data()), value);
}
