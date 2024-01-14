#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>
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

int main() {

    constexpr auto kWindowHeight { 600 };
    constexpr auto kWindowWidth { 800 };

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

    if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
        std::cerr << "Failed to initiliaze GLAD\n";
        return -1;
    }

    glViewport(0, 0, kWindowWidth, kWindowHeight);
    glfwSetFramebufferSizeCallback(window, frameBufferSizeCallback);

    while (!glfwWindowShouldClose(window)) {

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
