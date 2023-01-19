#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <emscripten/emscripten.h>

#include <iostream>

static void error_callback(int error, const char* description);
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void window_size_callback(GLFWwindow* window, int width, int height);

const int width = 640;
const int height = 480;

bool background_is_black = true;

extern "C" EMSCRIPTEN_KEEPALIVE
void toggle_background_color() {
    background_is_black = !background_is_black;
}

GLFWwindow* window;

void do_frame() {
    if (background_is_black) {
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    } else {
        glClearColor(1.0f, 0.5f, 0.5f, 1.0f);
    }

    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);
}

int main() {
    glfwSetErrorCallback(error_callback);

    if (!glfwInit()) {
        std::cout << "Failed to init GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(width, height, "My Window", NULL, NULL);
    if (!window) {
        std::cout << "Failed to create window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_size_callback);

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

static void window_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}
