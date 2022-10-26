#ifndef SHADER_H_
#define SHADER_H_

#include <glad/glad.h>

#include <string>

// Wrapper class
class Shader
{
    GLuint _handle;

    public:
        // Takes in vertex and fragment shader file paths
        Shader(const char* vShaderPath, const char* fShaderPath);

        // glUseProgram
        void use() { glUseProgram(_handle); }

        void setUniform(const std::string &name, bool value) const;
        void setUniform(const std::string &name, int value) const;
        void setUniform(const std::string &name, float value) const;
};

#endif // SHADER_H_
