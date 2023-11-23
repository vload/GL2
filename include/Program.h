#pragma once

#include <string>

#include "Shader.h"
#include "util.h"

class Program {
   private:
    unsigned int program;

   public:
    Program(Shader vertexShader, Shader fragmentShader) {
        program = glCreateProgram();
        glAttachShader(program, vertexShader.get());
        glAttachShader(program, fragmentShader.get());
        glLinkProgram(program);

        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
                      << infoLog << std::endl;
        }
    }

    Program(Shader vertexShader, Shader fragmentShader, Shader geometryShader) {
        program = glCreateProgram();
        glAttachShader(program, vertexShader.get());
        glAttachShader(program, fragmentShader.get());
        glAttachShader(program, geometryShader.get());
        glLinkProgram(program);

        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
                      << infoLog << std::endl;
        }
    }

    Program(Shader computeShader) {
        program = glCreateProgram();
        glAttachShader(program, computeShader.get());
        glLinkProgram(program);

        int success;
        char infoLog[512];
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(program, 512, NULL, infoLog);
            std::cout << "ERROR::SHADER::PROGRAM::LINK_FAILED\n"
                      << infoLog << std::endl;
        }
    }

    void use() { glUseProgram(program); }

    void set_uniform(std::string name, glm::vec4 v) {
        int location = glGetUniformLocation(program, name.c_str());
        glUniform4f(location, v.x, v.y, v.z, v.w);
    }

    void set_uniform(std::string name, glm::mat4 m) {
        int location = glGetUniformLocation(program, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(m));
    }

    void set_uniform(std::string name, int i) {
        int location = glGetUniformLocation(program, name.c_str());
        glUniform1i(location, i);
    }

    void set_uniform(std::string name, float f) {
        int location = glGetUniformLocation(program, name.c_str());
        glUniform1f(location, f);
    }

    unsigned int get() { return program; }
};