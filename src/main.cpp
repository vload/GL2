#include <iostream>  // for debugging purposes

#include "Program.h"
#include "Shader.h"
#include "WindowContext.h"
#include "util.h"

constexpr int num_balls = 100000;
constexpr float zoom = 10.0f;
constexpr float ball_size = 0.002f;

glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

void framebuffer_size_callback_ortho(GLFWwindow* window, int width,
                                     int height) {
    glViewport(0, 0, width, height);

    // Adjust the orthographic projection matrix based on the new aspect ratio
    float left = -1.0f;
    float right = 1.0f;
    float bottom = -1.0f;
    float top = 1.0f;

    // Calculate the new aspect ratio
    float aspectRatio = static_cast<float>(width) / static_cast<float>(height);

    // Adjust the orthographic projection matrix based on the new aspect ratio
    if (aspectRatio >= 1.0f) {
        // If the width is greater than or equal to the height
        left *= aspectRatio;
        right *= aspectRatio;
    } else {
        // If the height is greater than the width
        bottom /= aspectRatio;
        top /= aspectRatio;
    }

    // Recreate the orthographic projection matrix
    projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
}

struct ball {
    float x;
    float y;
    float vx;
    float vy;
} balls[num_balls];

void generate_balls() {
    // random lambda
    auto random = []() -> float {
        return (float)rand() / (float)RAND_MAX * 2.0f - 1.0f;
    };

    for (int i = 0; i < num_balls; i++) {
        balls[i].x = random() * 10;
        balls[i].y = random() * 10;
        balls[i].vx = random();
        balls[i].vy = random();
    }
}

int main() {
    WindowContext window = WindowContext();
    window.set_framebuffer_size_callback(framebuffer_size_callback_ortho);

    // Shader compilation
    Shader testVertexShader("shaders/test.vert", GL_VERTEX_SHADER);
    Shader testFragmentShader("shaders/test.frag", GL_FRAGMENT_SHADER);
    Shader geometryShader("shaders/test.geom", GL_GEOMETRY_SHADER);

    Shader computeShader("shaders/hello.comp", GL_COMPUTE_SHADER);

    // Program setup
    Program geometryProgram(testVertexShader, testFragmentShader,
                            geometryShader);
    Program computeProgram(computeShader);

    generate_balls();

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // set up vertex buffer with ball data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(balls), balls, GL_DYNAMIC_DRAW);

    // postions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ball), (void*)0);

    // velocities
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(ball),
                          (void*)(sizeof(float) * 2));

    // set up compute shader buffers
    enum SSBO { input, output, numSSBOs };
    unsigned int SSBOs[numSSBOs];
    glGenBuffers(numSSBOs, SSBOs);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[SSBO::input]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(balls), balls,
                 GL_DYNAMIC_DRAW);

    glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[SSBO::output]);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(balls), nullptr,
                 GL_DYNAMIC_DRAW);

    int frameCount = 0;

    // Enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    double old_time = glfwGetTime();
    double delta_time = 0.0f;

    while (!window.should_close()) {
        delta_time = glfwGetTime() - old_time;
        old_time = glfwGetTime();
        frameCount++;

        // process_input(window);

        computeProgram.use();
        // bind buffers
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBOs[SSBO::input]);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBOs[SSBO::output]);

        // set delta time
        computeProgram.set_uniform("delta_t", (float)delta_time);

        glDispatchCompute(num_balls, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Copy the results from the GPU back to the CPU
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[SSBO::output]);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(balls), balls);

        // render
        glClearColor(0.0f, 0.0f, 0.0f, 0.1f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 trans(1.0f);
        trans = glm::scale(trans, glm::vec3(0.05, 0.05, 0.05));

        geometryProgram.use();
        geometryProgram.set_uniform("projection", projection);
        geometryProgram.set_uniform(
            "zoom", glm::ortho(-zoom, zoom, -zoom, zoom, -1.0f, 1.0f));
        geometryProgram.set_uniform("size", ball_size);
        glBindVertexArray(VAO);
        glDrawArrays(GL_POINTS, 0, num_balls);

        // check and call events and swap the buffers
        window.swap_buffers();
        glfwPollEvents();

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(balls), balls, GL_DYNAMIC_DRAW);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[SSBO::input]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(balls), balls,
                     GL_DYNAMIC_DRAW);

        if (frameCount % 100 == 0) {
            std::cout << frameCount / glfwGetTime() << std::endl;
        }
    }

    return 0;
}

// void process_input(WindowContext& window) {
//     if (window.get_key(GLFW_KEY_ESCAPE) == GLFW_PRESS) {
//         window.set_should_close(true);
//     }
// }