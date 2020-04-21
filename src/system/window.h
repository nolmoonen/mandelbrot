#ifndef BOMBER_WINDOW_H
#define BOMBER_WINDOW_H

#include <stdint.h>
#include <GLFW/glfw3.h>

GLFWwindow *window;

extern const uint32_t INITIAL_WIDTH;
extern const uint32_t INITIAL_HEIGHT;

uint32_t window_width;
uint32_t window_height;

/** Call to {@link cleanup_window} is required if EXIT_SUCCESS is returned. */
int init_window();

int cleanup_window();

bool window_should_close();

int swap_window_buffers();

/** {@param t_title} needs to be null-terminated */
int set_window_title(const char *t_title);

uint32_t get_window_width();

uint32_t get_window_height();

/**
 * Begin GLFW callbacks.
 */

void error_callback(int t_error, const char *t_description);

static void key_callback(GLFWwindow *t_window, int t_key, int t_scancode, int t_action, int t_mods);

void scroll_callback(GLFWwindow *t_window, double t_xoffset, double t_yoffset);

void mouse_button_callback(GLFWwindow *t_window, int t_button, int t_action, int t_mods);

static void cursor_position_callback(GLFWwindow *t_window, double t_xpos, double t_ypos);

void framebuffer_size_callback(GLFWwindow *t_window, int t_width, int t_height);

/**
 * End GLFW callbacks.
 */

#endif //BOMBER_WINDOW_H
