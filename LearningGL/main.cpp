#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/mat3x3.hpp>
#include <glm/ext/matrix_transform.hpp>

#include <iostream>
#include <format>
#include <algorithm>
#include <memory>
#include <string_view>

void frameBufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window) {
    static size_t mouseCounter { 0 };
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

GLuint makeShader(const char* const *shaderSource, size_t sourceSize, GLenum shaderType) {
    auto shader { glCreateShader(shaderType) };
    glShaderSource(shader, 1, shaderSource, nullptr);

    int success {};
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char logBuffer[512];
        glGetShaderInfoLog(shader, std::size(logBuffer), nullptr, logBuffer);
        std::cerr << "Shader Failed: " << logBuffer << std::endl;
    }
    return shader;
}

int main() {
    // The constant values for window dimensions.
    constexpr auto kWindowHeight { 800 };
    constexpr auto kWindowWidth { 800 };
    // The test shader.
    const char* vertexShaderSource = "#version 330 core\n"
        "layout (location = 0) in vec3 aPos;\n"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
        "}\0";
    const char * fragmentShaderSource = "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main() {\n"
        "   FragColor = vec4(0.8f, 0.0f, 0.0f, 1.0f);\n"
    "}\0";

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

    auto vertexShader { glCreateShader(GL_VERTEX_SHADER) };
    glShaderSource(vertexShader, 1, &vertexShaderSource, nullptr);
    glCompileShader(vertexShader);
    int success {};
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infoLog[512];
        glGetShaderInfoLog(vertexShader, 512, nullptr, infoLog);
        std::cerr << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << '\n';
        return -1;
    }

    auto fragmentShader { glCreateShader(GL_FRAGMENT_SHADER) };
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, nullptr);
    glCompileShader(fragmentShader);
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        char infolog[512];
        glGetShaderInfoLog(fragmentShader, 512, nullptr, infolog);
        std::cerr << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infolog << '\n';
    }

    auto shaderProgram { glCreateProgram() };

    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);
    

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    unsigned int VBO {}, VAO {};
    // The main event loop.
    float angle { 0.0f };
    while (!glfwWindowShouldClose(window)) {

        glClear(GL_COLOR_BUFFER_BIT);

        double mouseX {}, mouseY {};
        glfwGetCursorPos(window, &mouseX, &mouseY);
        auto posX { std::clamp(static_cast<int>(mouseX), 0, kWindowWidth) };
        auto posY { std::clamp(static_cast<int>(mouseY), 0, kWindowHeight) };
        
        float scaleFactorX { static_cast<float>(posX) / kWindowWidth};
        float scaleFactorY { static_cast<float>(posY) / kWindowHeight};
                                                                                         
        angle = glm::atan((kWindowHeight / 2.0f - posY ) / (posX - kWindowWidth / 2.0f)) + glm::pi<float>() / 6.0f;
        if (posX < kWindowWidth / 2) {
            angle += glm::pi<float>();
        }

        std::cout << std::format("Angle: {:.2f}\n", angle);
        //std::cout << std::format("ScaleFactorX: {:.2f},\tScaleFactorY{:.2f}\n", scaleFactorX, scaleFactorY);

        const glm::mat3x3 rotationMatrix { {glm::cos(angle), glm::sin(angle), 0},
                                     {-1*glm::sin(angle), glm::cos(angle)   , 0},
                                     {0              , 0                 , 1} };

        glm::vec3 verticies[3] { glm::vec3(0.0f, 0.6f, 0.0f),
                                 glm::vec3(-1.0f * 0.5f, -0.3f, 0.0f),
                                 glm::vec3(0.5f, -0.3f, 0.0f) };

        for (int i = 0; i < 3; i++) {
            verticies[i] = rotationMatrix * verticies[i];
        }

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);             

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 3, verticies, GL_DYNAMIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);
        glEnableVertexAttribArray(0);

        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);


        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    // Clean up for resources.
    glfwTerminate();
    return 0;
}
