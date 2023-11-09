#pragma once

#include "util.h"

class Shader {
   private:
    unsigned int shader;

   public:
    Shader(const char* shader_path, unsigned int shader_type) {
        const char* shaderSource = read_file(shader_path);
        shader = glCreateShader(shader_type);
        glShaderSource(shader, 1, &shaderSource, NULL);
        glCompileShader(shader);

        int success;
        char infoLog[512];
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::" << shader_path
                      << "::COMPILATION_FAILED\n"
                      << infoLog << std::endl;
        }
    }

    // ~Shader() { glDeleteShader(shader); }

    unsigned int get() { return shader; }
};