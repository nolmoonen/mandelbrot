#include <stdlib.h>
#include <stdbool.h>
#include <glad/glad.h>

#include "window.h"
#include "util/log.h"
#include "input.h"

const uint32_t INITIAL_WIDTH = 800;
const uint32_t INITIAL_HEIGHT = 600;

int init_window() {
    glfwSetErrorCallback(error_callback);

    if (glfwInit() == GLFW_FALSE) {
        nm_log(LOG_ERROR, "failed to initialize GLFW\n");
        goto err_glfw;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    if ((window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "", NULL, NULL)) == NULL) {
        nm_log(LOG_ERROR, "failed to create window or OpenGl context\n");
        goto err_window;
    }

    window_width = INITIAL_WIDTH;
    window_height = INITIAL_HEIGHT;

    glfwSetKeyCallback(window, key_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    glfwMakeContextCurrent(window);

    if (gladLoadGL() == 0) {
        nm_log(LOG_ERROR, "failed to load OpenGL extensions\n");
        goto err_glad;
    }

    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);

    return EXIT_SUCCESS;

    err_glad:
    glfwDestroyWindow(window);
    err_window:
    glfwTerminate();
    err_glfw:

    return EXIT_FAILURE;
}

int cleanup_window() {
    glfwDestroyWindow(window);

    glfwTerminate();

    return EXIT_SUCCESS;
}

bool window_should_close() {
    return glfwWindowShouldClose(window) == GLFW_TRUE;
}

int swap_window_buffers() {
    glfwSwapBuffers(window);

    return EXIT_SUCCESS;
}

int set_window_title(const char *t_title) {
    glfwSetWindowTitle(window, t_title);

    return EXIT_SUCCESS;
}

void error_callback(int t_error, const char *t_description) {
    nm_log(LOG_ERROR, "glfw: %s\n", t_description);
}

void key_callback(GLFWwindow *t_window, int t_key, int t_scancode, int t_action, int t_mods) {
    if (t_key == GLFW_KEY_ESCAPE && t_action == GLFW_PRESS) {
        glfwSetWindowShouldClose(t_window, GLFW_TRUE);
    }

    switch (t_key) {
        case GLFW_KEY_ESCAPE:
            if (t_action == GLFW_PRESS) {
                glfwSetWindowShouldClose(t_window, GLFW_TRUE);
            }
            break;
        case GLFW_KEY_W:
            set_w_down(t_action == GLFW_PRESS || t_action == GLFW_REPEAT);
            break;
        case GLFW_KEY_A:
            set_a_down(t_action == GLFW_PRESS || t_action == GLFW_REPEAT);
            break;
        case GLFW_KEY_S:
            set_s_down(t_action == GLFW_PRESS || t_action == GLFW_REPEAT);
            break;
        case GLFW_KEY_D:
            set_d_down(t_action == GLFW_PRESS || t_action == GLFW_REPEAT);
            break;
        default:
            break;
    }
}

void scroll_callback(GLFWwindow *t_window, double t_xoffset, double t_yoffset) {
    set_scroll_offset(t_xoffset, t_yoffset);
}

void mouse_button_callback(GLFWwindow *t_window, int t_button, int t_action, int t_mods) {
    switch (t_button) {
        case GLFW_MOUSE_BUTTON_RIGHT:
            if (t_action == GLFW_PRESS) {
                right_pressed = true;
            } else if (t_action == GLFW_RELEASE) {
                right_pressed = false;
            }
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            if (t_action == GLFW_PRESS) {
                middle_pressed = true;
            } else if (t_action == GLFW_RELEASE) {
                middle_pressed = false;
            }
            break;
        default:
            break;
    }
}

void cursor_position_callback(GLFWwindow *t_window, double t_xpos, double t_ypos) {
    set_cursor_position(t_xpos, t_ypos);
}

void framebuffer_size_callback(GLFWwindow *t_window, int t_width, int t_height) {
    glViewport(0, 0, t_width, t_height);
}

uint32_t get_window_width() {
    return window_width;
}

uint32_t get_window_height() {
    return window_height;
}
