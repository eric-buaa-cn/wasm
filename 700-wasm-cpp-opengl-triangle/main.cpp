#include <GLES3/gl3.h>
#include <GLES3/gl2ext.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <emscripten/emscripten.h>

#include <glslprogram.h>

#include <iostream>

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void frame_size_callback(GLFWwindow* window, int width, int height);

const int width = 640;
const int height = 480;

bool background_is_black = true;

extern "C" EMSCRIPTEN_KEEPALIVE
void toggle_background_color() {
    background_is_black = !background_is_black;
}

GLSLProgram program;
GLFWwindow* window;
unsigned int VAO;
glm::vec4 uColor = {1.0f, 0.5f, 0.2f, 1.0f};

void do_frame() {
    if (background_is_black) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    }
    glClear(GL_COLOR_BUFFER_BIT);

    program.use();
    program.setUniform("uColor", uColor);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);


    glfwSwapBuffers(window);
    glfwPollEvents();
}

int main() {
    std::cout << "enter main" << std::endl;
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cout << "Failed to init GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
//    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
//#ifdef __APPLE__
//    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
//#endif

    window = glfwCreateWindow(width, height, "My Window", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, frame_size_callback);

    glm::vec3 vertices[] = {
        {1.0f, -1.0f, 0.0f},
        {-1.0f, -1.0f, 0.0f},
        {0, 1, 0.0f}
    };

//    glEnable(GL_MULTISAMPLE);
//    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    glGenVertexArrays(1, &VAO);
    std::cout << "VAO: " << VAO << std::endl;

    GLuint VBO;
    glGenBuffers(1, &VBO);
    std::cout << "VBO: " << VBO << std::endl;

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), glm::value_ptr(vertices[0]), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    if (!program.compileShader("draw_with_shader.vs")) {
        std::cout << "compile error" << std::endl;
        return -1;
    }
    if (!program.compileShader("draw_with_shader.fs")) {
        std::cout << "compile error" << std::endl;
        return -1;
    }

    if (!program.link()) {
        std::cout << "fail to link" << std::endl;
        return -1;
    }
    if (!program.validate()) {
        std::cout << "fail to validate" << std::endl;
        return -1;
    }

    glfwSwapInterval(1);
    emscripten_set_main_loop(do_frame, 0, 1);

    glfwDestroyWindow(window);
    glfwTerminate();
}

static void error_callback(int error, const char* description) {
    std::cout << "Error: " << error << ", Message: " << description << std::endl;
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static void frame_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
