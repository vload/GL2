#include <iostream>  // for debugging purposes

#include "Program.h"
#include "Shader.h"
#include "util.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

int main() {
    // Initialize GLFW
    glfwInit();

    // Tell GLFW what version of OpenGL we are using
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create a GLFW window object
    GLFWwindow* window = glfwCreateWindow(800, 600, "Hello", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD before calling any OpenGL function
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // Tell OpenGL the size of the rendering window
    glViewport(0, 0, 800, 800);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // Shader compilation
    Shader vertexShader("shaders/hello.vert", GL_VERTEX_SHADER);
    Shader fragmentShader("shaders/hello.frag", GL_FRAGMENT_SHADER);
    Shader blueFragmentShader("shaders/blue.frag", GL_FRAGMENT_SHADER);

    // Program setup
    Program program(vertexShader, fragmentShader);
    Program blueProgram(vertexShader, blueFragmentShader);

    float triangle1[] = {
        0.5f, 0.5f,  0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        0.2f, 0.0f,  0.0f,  // bottom left
    };
    float triangle2[] = {
        -0.5f, -0.5f, 0.0f,  // top left
        -0.5f, 0.5f,  0.0f,  // bottom left
        -0.2f, 0.0f,  0.0f   // top left
    };

    // generate a vertex buffer object (VBO) and vertex array object (VAO)
    unsigned int VBOs[2];
    glGenBuffers(2, VBOs);
    unsigned int VAOs[2];
    glGenVertexArrays(2, VAOs);

    // Triangle 1
    glBindVertexArray(VAOs[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle1), triangle1, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // Triangle 2
    glBindVertexArray(VAOs[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(triangle2), triangle2, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    int frameCount = 0;

    while (!glfwWindowShouldClose(window)) {
        frameCount++;
        processInput(window);

        // render
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        // glClear(GL_COLOR_BUFFER_BIT);

        float timeValue = glfwGetTime();
        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::rotate(trans, glm::radians(timeValue * 30.0f),
                            glm::vec3(0.0, 0.0, 1.0));
        trans = glm::translate(trans, glm::vec3(0.1, 0.1, 0.0));
        trans = glm::rotate(trans, glm::radians(timeValue * 50.0f),
                            glm::vec3(0.0, 0.0, 1.0));
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        float greenValue = (sin(timeValue) / 2.0f) + 0.5f;

        program.use();
        program.set_uniform(
            "ourColor", glm::vec4(0.0f, greenValue, 1.0f - greenValue, 1.0f));
        program.set_uniform("transform", trans);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        blueProgram.use();
        glBindVertexArray(VAOs[1]);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
        if (frameCount % 500 == 0)
            std::cout << frameCount / timeValue << std::endl;
    }

    glfwTerminate();
    return 0;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}