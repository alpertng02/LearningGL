#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/mat3x3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <array>
#include <algorithm>
#include <format>

#include <Shader.h>

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    static size_t mouseCounter { 0 };
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

struct Vertex {
    glm::vec3 pos {};
    glm::vec3 color {};
};

class Triangle {
public:
    Triangle(const Vertex& vertex1,const Vertex& vertex2, const Vertex& vertex3) : mVerticies { vertex1, vertex2, vertex3 } {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVerticies.size() * sizeof(Vertex), mVerticies.data(), GL_DYNAMIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (const void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
    }

    void draw() {



        glBindVertexArray(mVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }

    void rotate(const float angleRad) {

        float centerX {};
        float centerY {};
        for (auto& vertex : mVerticies) {
            centerX += vertex.pos.x;
            centerY += vertex.pos.y;
        }
        centerX /= 3;
        centerY /= 3;

        const glm::mat3x3 translationMatrix1 { {1,        0,        0},
                                               {0,        1,        0},
                                               {-centerX, -centerY, 1} };
        const glm::mat3x3 translationMatrix2 { {1,        0,        0},
                                               {0,        1,        0},
                                               {centerX,  centerY,  1} };

        const glm::mat3x3 rotationMatrix { {glm::cos(angleRad)     , glm::sin(angleRad), 0},
                                           {-glm::sin(angleRad)    , glm::cos(angleRad), 0},
                                           {0                      , 0                 , 1} };

        const glm::mat3x3 matrixT { translationMatrix2 * rotationMatrix * translationMatrix1 };

        for (auto& vertex : mVerticies) {
            vertex.pos = matrixT * vertex.pos;
        }
    }

    void translate(const float moveX, const float moveY) {
        for (auto& vertex : mVerticies) {
            vertex.pos.x += moveX;
            vertex.pos.y += moveY;
        }
    }

    ~Triangle() {
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
    }

private :
    std::array<Vertex, 3> mVerticies {};
    unsigned int mVAO {};
    unsigned int mVBO {};
};

void rotateTriangle(std::array<glm::vec3, 3>& verticies, float angle) {
    const glm::mat3x3 rotationMatrix { {glm::cos(angle), glm::sin(angle), 0},
                             {-1 * glm::sin(angle), glm::cos(angle)   , 0},
                             {0              , 0                 , 1} };

    for (auto& verticy : verticies) {
        verticy = rotationMatrix * verticy;
    }

}

void moveTriangle(std::array<glm::vec3, 3>& verticies, float x, float y) {

    for (auto& verticy : verticies) {
        verticy.x += x;
        verticy.y += y;
    }

}


int main() {
    // The constant values for window dimensions.
    constexpr auto kWindowHeight { 800 };
    constexpr auto kWindowWidth { 800 };

    // Initiliaze and create the GLFW window.
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    auto window { glfwCreateWindow(kWindowWidth, kWindowHeight, "My Test Window", nullptr, nullptr) };

    if (window == nullptr) {
        std::cerr << "Failed to create window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Load the GLAD libarary (This needs to be called after glfwMakeContextCurrent()!!)
    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initiliaze GLAD\n";
        return -1;
    }
    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << '\n';
    // Set the OpenGL viewport size and assign the callback function so that the size is always the same as window size.
    glViewport(0, 0, kWindowWidth, kWindowHeight);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);
    
    Shader ourShader("Shaders/basic.vert", "Shaders/basic.frag");
    ourShader.use();

    // The main event loop.
    constexpr float kMoveSpeed { 1.0f / 1.0f };
    float moveX { 0 }, moveY { 0 };
    double startTime { glfwGetTime() };
    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        const double currentTime { glfwGetTime() };
        const float moveDistance { static_cast<float>(kMoveSpeed * (currentTime - startTime)) };
        startTime = currentTime;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            moveY += moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            moveX -= moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            moveY -= moveDistance;
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            moveX += moveDistance;
        }

        double mouseX {}, mouseY {};
        glfwGetCursorPos(window, &mouseX, &mouseY);
        const auto posX { std::clamp(static_cast<int>(mouseX), 0, kWindowWidth) };
        const auto posY { std::clamp(static_cast<int>(mouseY), 0, kWindowHeight) };


        //angle = glm::atan((kWindowHeight / 2 - posY - moveY * kWindowHeight / 2) / (posX - kWindowWidth / 2 - moveX * kWindowWidth / 2)) + glm::pi<float>() / 6.0f;
        //if (posX < kWindowWidth / 2 + kWindowWidth / 2 * moveX) {
        //    angle += glm::pi<float>();
        //}  

        double angle = glm::atan(static_cast<float>(kWindowHeight / 2 - posY) / (posX - kWindowWidth / 2));
        if (posX < kWindowWidth / 2) {
            angle += glm::pi<double>();
        }

        std::cout << std::format("Angle: {:.2f}\n", angle);

        constexpr Vertex vertex1 { glm::vec3(0.5f, -0.5f, 1.0f),  glm::vec3(1.0f, 0.0f, 0.0f) };
        constexpr Vertex vertex2 { glm::vec3(-0.5f, -0.5f, 1.0f),  glm::vec3(0.0f, 1.0f, 0.0f) };
        constexpr Vertex vertex3 { glm::vec3(0.0f,  0.5f, 1.0f),  glm::vec3(0.0f, 0.0f, 1.0f) };

        Triangle triangle(vertex1, vertex2, vertex3);


        triangle.rotate(angle);
        triangle.translate(moveX, moveY);

        triangle.draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Clean up for resources.
    glfwTerminate();
    return 0;
}
