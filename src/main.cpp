#include <iostream>  // for debugging purposes

#include "Program.h"
#include "Shader.h"
#include "WindowContext.h"
#include "util.h"

#define PI 3.14159265358979323846

constexpr int num_balls = 1'000'000;
float zoom = 10.0f;
float ball_size = 0.002f;
float time_scale = 1.0f;

glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);

const float screenRectangleVertices[] = {
    // Coords       // TexCoords
    -1.0f, 1.0f,  0.0f, 1.0f,  // top left
    -1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
    1.0f,  -1.0f, 1.0f, 0.0f,  // bottom right

    -1.0f, 1.0f,  0.0f, 1.0f,  // top left
    1.0f,  -1.0f, 1.0f, 0.0f,  // bottom right
    1.0f,  1.0f,  1.0f, 1.0f   // top right
};

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
    float rangle = 0.0;
    float gangle = glm::radians(15.0);
    float bangle = glm::radians(45.0);
    int circle_count = 80;  // per 1 unit (1000 px)

    WindowContext window = WindowContext();
    window.set_framebuffer_size_callback(framebuffer_size_callback_ortho);

    // Shader compilation
    Shader testVertexShader("shaders/test.vert", GL_VERTEX_SHADER);
    Shader testFragmentShader("shaders/test.frag", GL_FRAGMENT_SHADER);
    Shader geometryShader("shaders/test.geom", GL_GEOMETRY_SHADER);

    Shader framebufferVertexShader("shaders/framebuffer.vert",
                                   GL_VERTEX_SHADER);
    Shader framebufferFragmentShader("shaders/framebuffer.frag",
                                     GL_FRAGMENT_SHADER);

    Shader computeShader("shaders/hello.comp", GL_COMPUTE_SHADER);

    // Program setup
    Program geometryProgram(testVertexShader, testFragmentShader,
                            geometryShader);
    Program computeProgram(computeShader);
    Program framebufferProgram(framebufferVertexShader,
                               framebufferFragmentShader);

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

    // set up framebuffer
    unsigned int FBO;
    glGenFramebuffers(1, &FBO);
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);

    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, window.get_width(),
                 window.get_height(), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D,
                           texture, 0);

    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8,
                          window.get_width(), window.get_height());
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT,
                              GL_RENDERBUFFER, RBO);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!"
                  << std::endl;
    }

    // set up screen rectangle
    unsigned int rectVAO;
    glGenVertexArrays(1, &rectVAO);
    unsigned int rectVBO;
    glGenBuffers(1, &rectVBO);

    glBindVertexArray(rectVAO);
    glBindBuffer(GL_ARRAY_BUFFER, rectVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenRectangleVertices),
                 screenRectangleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    // Enable blending and set clear color
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    double old_time = glfwGetTime();
    double delta_time = 0.0f;

    while (!window.should_close()) {
        // imgui new frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // imgui window
        // ImGui::ShowDemoWindow();  // Show demo window! :)
        ImGui::Text("Frame time: %lf", delta_time);

        ImGui::SliderFloat("ball size", &ball_size, 0.001f, 0.1f,
                           "ball size = %.3f");
        ImGui::SliderFloat("world size", &zoom, 1.0f, 100.f,
                           "world size = %.1f");
        ImGui::SliderFloat("time scale", &time_scale, 0.1f, 100.f,
                           "time scale = %.2f");

        ImGui::SliderFloat("red angle", &rangle, 0.0f, 2 * PI,
                           "red angle = %.2f");
        ImGui::SliderFloat("green angle", &gangle, 0.0f, 2 * PI,
                            "green angle = %.2f");
        ImGui::SliderFloat("blue angle", &bangle, 0.0f, 2 * PI,
                            "blue angle = %.2f");
        ImGui::SliderInt("circle count", &circle_count, 1, 1000,
                            "circle count = %d");
        if (ImGui::Button("Reset")) {
            generate_balls();
        }

        // process_input(window);

        //// Step 1: COMPUTE
        computeProgram.use();
        // bind buffers
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, SSBOs[SSBO::input]);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(balls), balls,
                     GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, SSBOs[SSBO::output]);

        // set delta time
        computeProgram.set_uniform("delta_t", (float)delta_time * time_scale);

        glDispatchCompute(num_balls, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        // Copy the results from the GPU back to the CPU
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, SSBOs[SSBO::output]);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(balls), balls);

        //// Step 2: RENDERING to framebuffer texture
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);

        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 trans(1.0f);
        trans = glm::scale(trans, glm::vec3(0.05, 0.05, 0.05));

        geometryProgram.use();

        // set uniforms
        geometryProgram.set_uniform("projection", projection);
        geometryProgram.set_uniform(
            "zoom", glm::ortho(-zoom, zoom, -zoom, zoom, -1.0f, 1.0f));
        geometryProgram.set_uniform("size", ball_size);
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(balls), balls, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_POINTS, 0, num_balls);

        // Step 3: RENDER to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        framebufferProgram.use();
        framebufferProgram.set_uniform("screen_texture", 0);
        framebufferProgram.set_uniform("rangle", rangle);
        framebufferProgram.set_uniform("gangle", gangle);
        framebufferProgram.set_uniform("bangle", bangle);
        framebufferProgram.set_uniform("circle_count", circle_count);
        glBindVertexArray(rectVAO);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // render imgui
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // check and call events and swap the buffers
        window.swap_buffers();
        glfwPollEvents();

        // time calculation and framrate computation
        delta_time = glfwGetTime() - old_time;
        old_time = glfwGetTime();
    }

    return 0;
}