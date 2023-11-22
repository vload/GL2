#pragma once

#include "util.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

class WindowContext {
   private:
    GLFWwindow* window;

   public:
    WindowContext() {
        // Initialize GLFW
        glfwInit();

        // Tell GLFW what version of OpenGL we are using
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        // Create a GLFW window object
        window = glfwCreateWindow(800, 800, "Hello", NULL, NULL);
        if (window == NULL) {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
            throw std::runtime_error("Failed to create GLFW window");
        }
        glfwMakeContextCurrent(window);

        // Initialize GLAD before calling any OpenGL function
        if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
            std::cout << "Failed to initialize GLAD" << std::endl;
            glfwTerminate();
            throw std::runtime_error("Failed to initialize GLAD");
        }

        // Tell OpenGL the size of the rendering window
        glViewport(0, 0, 800, 800);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    }

    ~WindowContext() { glfwTerminate(); }

    GLFWwindow* get() { return window; }

    void swap_buffers() { glfwSwapBuffers(window); }

    void poll_events() { glfwPollEvents(); }

    bool should_close() { return glfwWindowShouldClose(window); }

    void set_should_close(bool value) {
        glfwSetWindowShouldClose(window, value);
    }

    int get_key(int key) { return glfwGetKey(window, key); }

    int get_width() {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return width;
    }

    int get_height() {
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        return height;
    }
};
