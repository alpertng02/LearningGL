

#include <glad/glad.h>
#include <GLFW/glfw3.h>
   
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <Shader.h>
#include <glm/glm.hpp>

#include <iostream>
#include <array>
#include <vector>

extern "C" {
    _declspec(dllexport) uint32_t NvOptimusEnablement = 1;
    _declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);

// settings
constexpr unsigned int kScreenWidth = 800;
constexpr unsigned int kScreenHeight = 600;

struct Vertex {
    glm::vec3 pos {};
    glm::vec3 color {};
    glm::vec2 tex {};
};

class Triangle {
public:
    Triangle(const Vertex& vertex1, const Vertex& vertex2, const Vertex& vertex3) : mVerticies { vertex1, vertex2, vertex3 } {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO);

        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVerticies.size() * sizeof(Vertex), mVerticies.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void draw(const unsigned shaderProgram, const unsigned texture) {

        glUseProgram(shaderProgram);
        glUniform1i(glGetUniformLocation(shaderProgram, "ourTexture"), 0);
        glBindTexture(GL_TEXTURE_2D, texture);
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

private:
    std::array<Vertex, 3> mVerticies {};
    unsigned int mVAO {};
    unsigned int mVBO {};
};

class Rectangle {
public:
    Rectangle(const Vertex& vertex1, const Vertex& vertex2, const Vertex& vertex3, const Vertex& vertex4) : mVerticies { vertex1, vertex2, vertex3, vertex4 } {
        glGenVertexArrays(1, &mVAO);
        glGenBuffers(1, &mVBO); 
        glGenBuffers(1, &mEBO);

        glBindVertexArray(mVAO);

        glBindBuffer(GL_ARRAY_BUFFER, mVBO);
        glBufferData(GL_ARRAY_BUFFER, mVerticies.size() * sizeof(Vertex), mVerticies.data(), GL_STATIC_DRAW);

        constexpr unsigned int indices[] { 0, 1, 3,
                                           1, 2, 3 };
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mEBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, std::size(indices) * sizeof(unsigned int), indices, GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (const void*)(6 * sizeof(float)));
        glEnableVertexAttribArray(2);
    }

    void draw(const unsigned shaderProgram, const std::vector<unsigned int>& textures) {

        for (int i = 0; const auto & texture : textures) {
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_2D, texture);
            i++;
        }
        glUseProgram(shaderProgram);

        glBindVertexArray(mVAO);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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

    ~Rectangle() {
        glDeleteVertexArrays(1, &mVAO);
        glDeleteBuffers(1, &mVBO);
        glDeleteBuffers(1, &mEBO);
    }

private:
    std::array<Vertex, 4> mVerticies {};
    unsigned int mVAO {};
    unsigned int mVBO {};
    unsigned int mEBO {};
};

int main() {
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(kScreenWidth, kScreenHeight, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    GLFWimage images[1] {};
    images[0].pixels = stbi_load("Misc/dogeIcon.jpg", &images[0].width, &images[0].height, 0, 4);
    glfwSetWindowIcon(window, 1, images);
    stbi_image_free(images[0].pixels);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    const char* vendor { reinterpret_cast<const char*>(glGetString(GL_VENDOR)) };
    const char* renderer { reinterpret_cast<const char*>(glGetString(GL_RENDERER)) };

    std::cout << "Current OpenGL Vendor: " << vendor << '\n';
    std::cout << "Current OpenGL Renderer: " << renderer << '\n';

    // build and compile our shader program
    // ------------------------------------
    Shader shader("Misc/Shaders/texture.vert", "Misc/Shaders/texture.frag"); // you can name your shader files however you like

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    constexpr Vertex vertex1 { glm::vec3(0.5f, 0.5f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f) };
    constexpr Vertex vertex2 { glm::vec3(0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec2(1.0f, 0.0f) };
    constexpr Vertex vertex3 { glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec2(0.0f, 0.0f) };
    constexpr Vertex vertex4 { glm::vec3(-0.5f, 0.5f, 0.0f), glm::vec3(0.5f, 0.5f, 1.0f), glm::vec2(0.0f, 1.0f) };
    //Triangle triangle(vertex1, vertex2, vertex3);
    Rectangle rectangle(vertex1, vertex2, vertex3, vertex4);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    // glBindVertexArray(0);

    stbi_set_flip_vertically_on_load(true);
    unsigned int texture1 {};
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width {}, height {}, nrChannels {};
    unsigned char * data { stbi_load("Misc/Textures/container.jpg", &width, &height, &nrChannels, 0) };
    if (data == nullptr) {
        std::cerr << "Failed to load texture\n";
    }
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glGenerateMipmap(GL_TEXTURE_2D);

    unsigned int texture2 {};
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    data = stbi_load("Misc/Textures/awesomeface.png", &width, &height, &nrChannels, 0);
    if (data == nullptr) {
        std::cerr << "Failed to load texture\n";
    }
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    glGenerateMipmap(GL_TEXTURE_2D);

    shader.use();
    glUniform1i(glGetUniformLocation(shader.ID, "texture1"), 0);
    glUniform1i(glGetUniformLocation(shader.ID, "texture2"), 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window)) {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // render the triangle
    
        rectangle.draw(shader.ID, {texture1, texture2});

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}

