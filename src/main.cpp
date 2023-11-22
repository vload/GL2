#include <iostream>  // for debugging purposes

#include "Program.h"
#include "Shader.h"
#include "WindowContext.h"
#include "util.h"

#define SHOW_FPS

// define a function for random number generation from -1.0  to 1.0
float random() { return (float)rand() / (float)RAND_MAX * 2.0f - 1.0f; }

void process_input(WindowContext& window) {
    if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        window.set_should_close(true);
    }
}

int main() {
    WindowContext window = WindowContext();

    // Shader compilation
    Shader vertexShader("shaders/hello.vert", GL_VERTEX_SHADER);
    Shader fragmentShader("shaders/hello.frag", GL_FRAGMENT_SHADER);
    Shader blueFragmentShader("shaders/blue.frag", GL_FRAGMENT_SHADER);

    Shader testVertexShader("shaders/test.vert", GL_VERTEX_SHADER);
    Shader testFragmentShader("shaders/test.frag", GL_FRAGMENT_SHADER);
    Shader geometryShader("shaders/test.geom", GL_GEOMETRY_SHADER);

    // Program setup
    Program program(vertexShader, fragmentShader);
    Program blueProgram(vertexShader, blueFragmentShader);
    Program geometryProgram(testVertexShader, testFragmentShader,
                            geometryShader);

    constexpr int num_balls = 20;

    struct ball {
        float x;
        float y;
        float vx;
        float vy;
    } balls[num_balls];

    for (int i = 0; i < num_balls; i++) {
        balls[i].x = random();
        balls[i].y = random();
        balls[i].vx = random();
        balls[i].vy = random();
    }

    // generate a vertex buffer object (VBO) and vertex array object (VAO)
    enum VBO { locations, velocities };

    unsigned int VBOs[2];
    glGenBuffers(2, VBOs);
    unsigned int VAOs[2];
    glGenVertexArrays(2, VAOs);

    glBindVertexArray(VAOs[0]);
    // locations
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[VBO::locations]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(balls), balls, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ball), (void*)0);
    glEnableVertexAttribArray(0);

    // // velocities
    // glBindBuffer(GL_ARRAY_BUFFER, VBOs[VBO::velocities]);
    // glBufferData(GL_ARRAY_BUFFER, sizeof(balls), balls, GL_STATIC_DRAW);
    // glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ball),
    //                       (void*)(2 * sizeof(float)));
    // glEnableVertexAttribArray(1);

    int frameCount = 0;

    // Enable blending
    glEnable(GL_BLEND);
    // Set the blend function
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double old_time = glfwGetTime();
    double delta_time;

    while (!window.should_close()) {
        delta_time = glfwGetTime() - old_time;
        old_time = glfwGetTime();
        frameCount++;
        process_input(window);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        geometryProgram.use();
        geometryProgram.set_uniform("transform", trans);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_POINTS, 0, num_balls);

        // check and call events and swap the buffers
        window.swap_buffers();
        glfwPollEvents();

        // move balls according to their velocities and time passed
        for (int i = 0; i < num_balls; i++) {
            balls[i].x += balls[i].vx * delta_time;
            balls[i].y += balls[i].vy * delta_time;
            if (balls[i].x > 1.0f || balls[i].x < -1.0f) balls[i].vx *= -1;
            if (balls[i].y > 1.0f || balls[i].y < -1.0f) balls[i].vy *= -1;
        }

        glBindVertexArray(VAOs[0]);
        glBindBuffer(GL_ARRAY_BUFFER, VBOs[VBO::locations]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(balls), balls, GL_DYNAMIC_DRAW);

#ifdef SHOW_FPS
        if (frameCount % 500 == 0) {
            std::cout << frameCount / glfwGetTime() << std::endl;
        }
#endif
    }

    return 0;
}
