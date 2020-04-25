#include <stdlib.h>
#include <stdbool.h>
#include <glad/glad.h>

#include "window.h"
#include "util/log.h"
#include "input.h"

int init_window()
{
    glfwSetErrorCallback(error_callback);

    if (glfwInit() == GLFW_FALSE) {
        nm_log(LOG_ERROR, "failed to initialize GLFW\n");
        goto err_glfw;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    if ((m_window = glfwCreateWindow(INITIAL_WIDTH, INITIAL_HEIGHT, "", NULL, NULL)) == NULL) {
        nm_log(LOG_ERROR, "failed to create window or OpenGl context\n");
        goto err_window;
    }

    m_window_width = INITIAL_WIDTH;
    m_window_height = INITIAL_HEIGHT;

    glfwSetKeyCallback(m_window, key_callback);
    glfwSetScrollCallback(m_window, scroll_callback);
    glfwSetMouseButtonCallback(m_window, mouse_button_callback);
    glfwSetCursorPosCallback(m_window, cursor_position_callback);
    glfwSetFramebufferSizeCallback(m_window, framebuffer_size_callback);

    glfwMakeContextCurrent(m_window);

    if (gladLoadGL() == 0) {
        nm_log(LOG_ERROR, "failed to load OpenGL extensions\n");
        goto err_glad;
    }

    glfwSwapInterval(1);

    glEnable(GL_DEPTH_TEST);

    glClearColor(0.0f, 0.1f, 0.2f, 1.0f);

    return EXIT_SUCCESS;

    err_glad:
    glfwDestroyWindow(m_window);
    err_window:
    glfwTerminate();
    err_glfw:

    return EXIT_FAILURE;
}

int cleanup_window()
{
    glfwDestroyWindow(m_window);

    glfwTerminate();

    return EXIT_SUCCESS;
}

bool window_should_close()
{
    return glfwWindowShouldClose(m_window) == GLFW_TRUE;
}

void set_window_to_close()
{
    glfwSetWindowShouldClose(m_window, GLFW_TRUE);
}

int swap_window_buffers()
{
    glfwSwapBuffers(m_window);

    return EXIT_SUCCESS;
}

int set_window_title(const char *t_title)
{
    glfwSetWindowTitle(m_window, t_title);

    return EXIT_SUCCESS;
}

void error_callback(int t_error, const char *t_description)
{
    nm_log(LOG_ERROR, "glfw: %s\n", t_description);
}

void key_callback(GLFWwindow *t_window, int t_key, int t_scancode, int t_action, int t_mods)
{
    switch (t_key) {
        case GLFW_KEY_ESCAPE:
            set_esc_down(t_action == GLFW_PRESS || t_action == GLFW_REPEAT);
            break;
        case GLFW_KEY_BACKSPACE :
            set_bs_down(t_action == GLFW_PRESS || t_action == GLFW_REPEAT);
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

void scroll_callback(GLFWwindow *t_window, double t_xoffset, double t_yoffset)
{
    set_scroll_offset(t_xoffset, t_yoffset);
}

void mouse_button_callback(GLFWwindow *t_window, int t_button, int t_action, int t_mods)
{
    // per documentation: "The action is one of GLFW_PRESS or GLFW_RELEASE."
    switch (t_button) {
        case GLFW_MOUSE_BUTTON_LEFT:
            left_pressed = t_action == GLFW_PRESS;
            break;
        case GLFW_MOUSE_BUTTON_MIDDLE:
            middle_pressed = t_action == GLFW_PRESS;
            break;
        case GLFW_MOUSE_BUTTON_RIGHT:
            right_pressed = t_action == GLFW_PRESS;
            break;
        default:
            break;
    }
}

void cursor_position_callback(GLFWwindow *t_window, double t_xpos, double t_ypos)
{
    set_cursor_position(t_xpos, t_ypos);
}

void framebuffer_size_callback(GLFWwindow *t_window, int t_width, int t_height)
{
    glViewport(0, 0, t_width, t_height);
}

uint32_t get_window_width()
{
    return m_window_width;
}

uint32_t get_window_height()
{
    return m_window_height;
}
