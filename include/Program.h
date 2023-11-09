#pragma once

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

    //    ~Program() {  }

    unsigned int get() { return program; }
};