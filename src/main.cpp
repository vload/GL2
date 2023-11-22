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

float screenRectangleVertices[] = {
    // Coords       // TexCoords
    -1.0f, 1.0f,  0.0f, 1.0f,  // top left
    -1.0f, -1.0f, 0.0f, 0.0f,  // bottom left
    1.0f,  -1.0f, 1.0f, 0.0f,  // bottom right

    -1.0f, 1.0f,  0.0f, 1.0f,  // top left
    1.0f,  -1.0f, 1.0f, 0.0f,  // bottom right
    1.0f,  1.0f,  1.0f, 1.0f  // top right
};

int main() {
    WindowContext window = WindowContext();

    // Shader compilation
    Shader vertexShader("shaders/hello.vert", GL_VERTEX_SHADER);
    Shader fragmentShader("shaders/hello.frag", GL_FRAGMENT_SHADER);
    Shader blueFragmentShader("shaders/blue.frag", GL_FRAGMENT_SHADER);

    Shader testVertexShader("shaders/test.vert", GL_VERTEX_SHADER);
    Shader testFragmentShader("shaders/test.frag", GL_FRAGMENT_SHADER);
    Shader geometryShader("shaders/test.geom", GL_GEOMETRY_SHADER);

    Shader framebufferVertexShader("shaders/framebuffer.vert",
                                   GL_VERTEX_SHADER);
    Shader framebufferFragmentShader("shaders/framebuffer.frag",
                                     GL_FRAGMENT_SHADER);
    // Program setup
    Program program(vertexShader, fragmentShader);
    Program blueProgram(vertexShader, blueFragmentShader);
    Program geometryProgram(testVertexShader, testFragmentShader,
                            geometryShader);
    Program framebufferProgram(framebufferVertexShader,
                               framebufferFragmentShader);

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
    enum VAO { BALLS, rectangle, numVAOs };
    enum VBO { locations, velocities, rectangleBuf, numVBOs };

    unsigned int VAOs[numVAOs];
    glGenVertexArrays(numVAOs, VAOs);
    unsigned int VBOs[numVBOs];
    glGenBuffers(numVBOs, VBOs);

    glBindVertexArray(VAOs[VAO::BALLS]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[VBO::locations]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(balls), balls, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(ball), (void*)0);

    glBindVertexArray(VAOs[VAO::rectangle]);
    glBindBuffer(GL_ARRAY_BUFFER, VBOs[VBO::rectangleBuf]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenRectangleVertices),
                 screenRectangleVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float),
                          (void*)(2 * sizeof(float)));

    int frameCount = 0;

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

        // render to texture (framebuffer) for post-processing
        glBindFramebuffer(GL_FRAMEBUFFER, FBO);
        // render
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glm::mat4 trans = glm::mat4(1.0f);
        trans = glm::scale(trans, glm::vec3(0.5, 0.5, 0.5));

        geometryProgram.use();
        geometryProgram.set_uniform("transform", trans);
        glBindVertexArray(VAOs[0]);
        glDrawArrays(GL_POINTS, 0, num_balls);


        // render to screen
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        framebufferProgram.use();
        framebufferProgram.set_uniform("screen_texture", 0);
        glBindVertexArray(VAOs[VAO::rectangle]);
        glDisable(GL_DEPTH_TEST);
        glBindTexture(GL_TEXTURE_2D, texture);
        glDrawArrays(GL_TRIANGLES, 0, 6);

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
