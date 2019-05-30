#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "math.h"
#include "vulkan.h"
#include "complex.h"
#include "color.h"

// limit to the number of iterations in the mandelbrot function
#define MAX_ITER 80

// defines a fractal through coordinates
typedef struct Fractal {
    double re_start;
    double re_end;
    double im_start;
    double im_end;
} Fractal;

// mandelbrot variables
const Fractal FRACTAL_START = {
        -2, // RE_START
        1, // RE_END
        -1, // IM_START
        1, // IM_END
};

Texture *texture; // current texture

const uint32_t MAX_LEVELS = 20; // maximum amount of times that can be zoomed
Fractal *fractal_stack; // stack of fractals to go back once zoomed
uint32_t current_level; // current position on stack

uint32_t screen_width = WIDTH; // width of the screen
uint32_t screen_height = HEIGHT; // height of the screen

double xpos, ypos; // position of the mouse
double clicked_xpos, clicked_ypos; // position of the mouse pressed down

bool selecting = false; // whether something is being selected

static void error_callback(int error, const char *description);

static void key_callback(GLFWwindow *p_window, int key, int scancode, int action, int mods);

static void framebuffer_resize_callback(GLFWwindow *p_window, int p_width, int p_height);

static void cursor_position_callback(GLFWwindow *p_window, double pxpos, double pypos);

static void mouse_button_callback(GLFWwindow *p_window, int button, int action, int mods);

double linear_interpolation(double color1, double color2, double t) {
    return color1 * (1 - t) + color2 * t;
}

/**
 * Calculates a HSV color based on iterations.
 */
struct RGB color(double m, double *hues) {
    struct HSV hsv = {0};
    hsv.h = 255 -
            (uint32_t) (255 * linear_interpolation(hues[(uint32_t) floor(m)], hues[(uint32_t) ceil(m)], fmod(m, 1)));
    hsv.s = 255;
    hsv.v = m < MAX_ITER ? 255 : 0;
    struct RGB rgb = HSVtoRGB(hsv);
    return rgb;
}

/**
 * Calculates iterations to converge, given a complex number.
 */
double mandelbrot(struct Complex c) {
    struct Complex z = {0, 0};
    uint32_t n = 0;

    while (abs_complex(z) <= 2 && n < MAX_ITER) {
        z = add_complex(mul_complex(z, z), c);
        ++n;
    }

    if (n == MAX_ITER) {
        return MAX_ITER;
    }

    return n + 1 - log(log2(abs_complex(z)));
}

void generate(Texture *p_texture) {
    double *all_iterations = (double *) malloc(sizeof(double) * p_texture->width * p_texture->height);
    uint32_t histogram[MAX_ITER - 1] = {0};
    uint32_t total = 0;

    /*
     * calculate iterations
     */
    for (uint32_t x = 0; x < p_texture->width; ++x) {
        for (uint32_t y = 0; y < p_texture->height; ++y) {
            double re_diff =
                    fractal_stack[current_level].re_end - fractal_stack[current_level].re_start;
            double im_diff =
                    fractal_stack[current_level].im_end - fractal_stack[current_level].im_start;

            // convert pixel coordinate to complex number
            struct Complex c = {
                    fractal_stack[current_level].re_start + (x / (double) p_texture->width) * re_diff,
                    fractal_stack[current_level].im_start + (y / (double) p_texture->height) * im_diff,
            };

            // compute number of iterations
            double m = mandelbrot(c);
            all_iterations[y * p_texture->width + x] = m;

            if (m < MAX_ITER) {
                histogram[(uint32_t) floor(m)]++;
                total++;
            }
        }
    }

    double *hues = (double *) malloc(sizeof(double) * (MAX_ITER + 1));
    double h = 0;
    for (uint32_t i = 0; i < MAX_ITER; ++i) {
        h += histogram[i] / (double) total;
        hues[i] = h;
    }
    hues[MAX_ITER] = h;

    /*
     * calculate and set colors
     */
    for (uint32_t x = 0; x < p_texture->width; ++x) {
        for (uint32_t y = 0; y < p_texture->height; ++y) {
            // create color, based on the number of iterations
            struct RGB rgb = color(all_iterations[y * p_texture->width + x], hues);

            // fill data
            uint32_t pixel = (y * p_texture->width + x) * 4;
            p_texture->data[pixel + 0] = rgb.r;
            p_texture->data[pixel + 1] = rgb.g;
            p_texture->data[pixel + 2] = rgb.b;
            p_texture->data[pixel + 3] = 255; // a
        }
    }

    free(hues);
    free(all_iterations);
}

void generate_texture(Texture *p_texture, uint32_t p_width, uint32_t p_height) {
    p_texture->width = p_width;
    p_texture->height = p_height;
    p_texture->data = (uint8_t *) malloc(sizeof(uint8_t) * p_texture->width * p_texture->height * 4);
    generate(p_texture);
}

UniformBufferObject calculate_uniform_buffer_object() {
    Mat4f proj = MAT4F_IDENTITY;

    if (selecting) {
        int32_t selected_width = xpos - clicked_xpos;
        int32_t selected_height = ypos - clicked_ypos;

        /*
         * translate
         */
        proj = cross(proj, translate((Vec3f) {
                -1.0f + ((clicked_xpos * 2.0f + selected_width) / (float) screen_width),
                -1.0f + ((clicked_ypos * 2.0f + selected_height) / (float) screen_height),
                0.0f,
        }));

        /*
         * scaling
         */
        proj = cross(proj, scale((Vec3f) {
                selected_width / (float) screen_width,
                selected_height / (float) screen_height,
                1.0f,
        }));
    } else {
        // hide selected quad behind textured quad
        proj = cross(proj, translate((Vec3f) {
                0.0f,
                0.0f,
                -1.0f,
        }));
    }

    return (UniformBufferObject) {MAT4F_IDENTITY, MAT4F_IDENTITY, proj};
}

int main() {
    GLFWwindow *window;

    // initialize glfw
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Mandelbrot", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return 1;
    }

    // set callbacks
    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // set up coordinate stack
    fractal_stack = (Fractal *) malloc(sizeof(Fractal) * MAX_LEVELS);
    current_level = 0;
    fractal_stack[current_level] = FRACTAL_START;

    // generate first texture
    texture = (Texture *) malloc(sizeof(Texture));
    generate_texture(texture, WIDTH, HEIGHT);

    // initialize vulkan
    if (!vulkanInit(window, texture, &calculate_uniform_buffer_object)) {
        glfwTerminate();
        return 1;
    }

    // create circular buffer for title
    CircularTickBuffer *circular_buffer = (CircularTickBuffer *) malloc(sizeof(CircularTickBuffer));
    tick_buffer_init(circular_buffer, 20000, CLOCKS_PER_SEC);

    // main loop
    while (!glfwWindowShouldClose(window)) {
        tick_buffer_add(circular_buffer, clock());
        glfwPollEvents();
        drawFrame(texture);

        // create and display title
        char title[256];
        title[255] = '\0';
        snprintf(title, 255, "fps: %d", tick_buffer_query(circular_buffer, clock()));
        glfwSetWindowTitle(window, title);
    }

    // free allocated memory
    free(circular_buffer);
    free(texture);
    free(fractal_stack);

    // wait until vulkan can be terminated
    waitDeviceIdle();

    // clean up vulkan
    vulkanTerminate();

    // clean up glfw
    glfwDestroyWindow(window);
    glfwTerminate();
}

void recreate_and_submit_texture() {
    generate_texture(texture, screen_width, screen_height);
    recreateTexture();
}

static void key_callback(GLFWwindow *p_window, int key, int scancode, int action, int mods) {
    // escape closes the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(p_window, GLFW_TRUE);
    }

    // enter refreshes the texture
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        fprintf(stdout, "recreating texture..\n");
        recreate_and_submit_texture();
    }

    // backspace zooms out
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        if (current_level == 0) {
            fprintf(stdout, "cannot go back!\n");
        } else {
            current_level--;
            recreate_and_submit_texture();
        }
    }
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "error: %s\n", description);
}

static void cursor_position_callback(GLFWwindow *p_window, double pxpos, double pypos) {
    xpos = pxpos;
    ypos = pypos;
}

static void mouse_button_callback(GLFWwindow *p_window, int button, int action, int mods) {
    // cancel selection
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        if (selecting) {
            selecting = !selecting;
        }
    }

    // start selecting
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        clicked_xpos = xpos;
        clicked_ypos = ypos;

        selecting = true;
    }

    // confirm selection
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        if (current_level == MAX_LEVELS - 1) {
            fprintf(stdout, "maximum depth reached!\n");
        } else {
            if (selecting) { // to allow for canceling selection
                double re_diff = fractal_stack[current_level].re_end - fractal_stack[current_level].re_start;
                double im_diff = fractal_stack[current_level].im_end - fractal_stack[current_level].im_start;

                // new fractal definition
                Fractal new_pos = {
                        fractal_stack[current_level].re_start + (clicked_xpos / screen_width) * re_diff,
                        fractal_stack[current_level].re_end - ((screen_width - xpos) / screen_width) * re_diff,
                        fractal_stack[current_level].im_start + (clicked_ypos / screen_height) * im_diff,
                        fractal_stack[current_level].im_end - ((screen_height - ypos) / screen_height) * im_diff,
                };

                // add to next position on stack
                fractal_stack[++current_level] = new_pos;

                recreate_and_submit_texture();
            }
        }

        selecting = false;
    }
}

static void framebuffer_resize_callback(GLFWwindow *p_window, int p_width, int p_height) {
    // set new dimensions
    screen_width = p_width;
    screen_height = p_height;

    // let vulkan know to rebuild swap chain
    framebufferResized = true;
}