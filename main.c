#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "math.h"
#include "vulkan.h"
#include "bmp.h"
#include "complex.h"
#include "color.h"

// limit to the number of iterations in the mandelbrot function
const uint32_t MAX_ITER = 80;

// mandelbrot variables
const int32_t RE_START = -2;
const int32_t RE_END = 1;
const int32_t IM_START = -1;
const int32_t IM_END = 1;

uint32_t width = WIDTH; // width of the screen
uint32_t height = HEIGHT; // height of the screen

double xpos, ypos; // position of the mouse
double clicked_xpos, clicked_ypos; // position of the mouse pressed down
double release_xpos, release_ypos; // position of the mouse released

bool selecting = false; // whether something is being selected

static void error_callback(int error, const char *description);

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods);

static void framebuffer_resize_callback(GLFWwindow *window, int width, int height);

static void cursor_position_callback(GLFWwindow *window, double pxpos, double pypos);

static void mouse_button_callback(GLFWwindow *window, int button, int action, int mods);

/**
 * Calculates a HSV color based on iterations.
 */
struct RGB color(uint32_t n, uint32_t N) {
    struct HSV hsv = {0};
    hsv.h = 255 * n / N;
    hsv.s = 255;
    hsv.v = n < N ? 255 : 0;
    struct RGB rgb = HSVtoRGB(hsv);
    return rgb;
}

/**
 * Calculates a RGB color based on iterations and a specified base color.
 */
struct RGB basecolor(uint32_t n, uint32_t N) {
    const struct RGB BASE = {255, 255, 255};
    const uint32_t THRESHOLD = 2;

    if (n > N / THRESHOLD) {
        // middle, always black
        struct RGB rgb = {0, 0, 0};
        return rgb;
    } else {
        // (2 * n < N)
        // edges, change from black to blue
        struct RGB rgb = {(n * BASE.r / N) * THRESHOLD, (n * BASE.g / N) * THRESHOLD, (n * BASE.b / N) * THRESHOLD};
        return rgb;
    }
}

/**
 * Calculates iterations to converge, given a complex number.
 */
uint32_t mandelbrot(struct Complex c) {
    struct Complex z = {0, 0};
    uint32_t n = 0;

    while (abs_complex(z) <= 2 && n < MAX_ITER) {
        z = add_complex(mul_complex(z, z), c);
        ++n;
    }

    return n;
}

void generate(Texture *texture) {
    for (uint32_t x = 0; x < texture->width; ++x) {
        for (uint32_t y = 0; y < texture->height; ++y) {
            // convert pixel coordinate to complex number
            struct Complex c = {
                    RE_START + (x / (double) texture->width) * (RE_END - RE_START),
                    IM_START + (y / (double) texture->height) * (IM_END - IM_START)
            };

            // compute number of iterations
            uint32_t m = mandelbrot(c);

            // create color, based on the number of iterations
            struct RGB rgb = basecolor(m, MAX_ITER);

            // fill data
            *(texture->data + (y * texture->width + x) * 4 + 0) = rgb.r;
            *(texture->data + (y * texture->width + x) * 4 + 1) = rgb.g;
            *(texture->data + (y * texture->width + x) * 4 + 2) = rgb.b;
            *(texture->data + (y * texture->width + x) * 4 + 3) = 255; // a
        }
    }
}

void generateTexture(Texture *ptexture, uint32_t pwidth, uint32_t pheight) {
    ptexture->width = pwidth;
    ptexture->height = pheight;
    ptexture->data = (uint8_t *) malloc(sizeof(uint8_t) * ptexture->width * ptexture->height * 4);
    generate(ptexture);
}

UniformBufferObject calculateUniformBufferObject() {
    mat4f proj = MAT4F_IDENTITY;

    if (selecting) {
        int32_t selected_width = xpos - clicked_xpos;
        int32_t selected_height = ypos - clicked_ypos;

        /*
         * translate
         */
        proj = mul(proj, translate((vec3f) {
                -1.0f + ((clicked_xpos * 2.0f + selected_width) / (float) width),
                -1.0f + ((clicked_ypos * 2.0f + selected_height) / (float) height),
                0.0f,
        }));

        /*
         * scaling
         */
        proj = mul(proj, scale((vec3f) {
                selected_width / (float) width,
                selected_height / (float) height,
                1.0f,
        }));
    } else {
        // hide selected quad behind textured quad
        proj = mul(proj, translate((vec3f) {
                0.0f,
                0.0f,
                -1.0f,
        }));
    }

    return (UniformBufferObject) {MAT4F_IDENTITY, MAT4F_IDENTITY, proj};
}

Texture *texture;

int main() {
    GLFWwindow *window;

    // initialize glfw
    if (!glfwInit()) {
        return 1;
    }

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    window = glfwCreateWindow(WIDTH, HEIGHT, "Vulkan", NULL, NULL);

    if (!window) {
        glfwTerminate();
        return 1;
    }

    glfwSetErrorCallback(error_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // generate first texture
    texture = (Texture *) malloc(sizeof(Texture));
    generateTexture(texture, WIDTH, HEIGHT);

    // initialize vulkan
    if (!vulkanInit(window, texture, &calculateUniformBufferObject)) {
        glfwTerminate();
        return 1;
    }

    // draw the first frame
    drawFrame(texture);

    // create circular buffer for title
    CircularTickBuffer *circularBuffer = (CircularTickBuffer *) malloc(sizeof(CircularTickBuffer));
    tick_buffer_init(circularBuffer, 20000, CLOCKS_PER_SEC);

    // main loop
    while (!glfwWindowShouldClose(window)) {
        tick_buffer_add(circularBuffer, clock());
        glfwPollEvents();
        drawFrame(texture);

        // create and display title
        char title[256];
        title[255] = '\0';
        snprintf(title, 255, "fps: %d", tick_buffer_query(circularBuffer, clock()));
        glfwSetWindowTitle(window, title);
    }

    // wait until vulkan can be terminated
    waitDeviceIdle();

    // clean up vulkan
    vulkanTerminate();

    // clean up glfw
    glfwDestroyWindow(window);
    glfwTerminate();
}

static void key_callback(GLFWwindow *pwindow, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) glfwSetWindowShouldClose(pwindow, GLFW_TRUE);
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
        texture = (Texture *) malloc(sizeof(Texture));
        generateTexture(texture, width, height);

        recreateTexture();
    }
}

static void error_callback(int error, const char *description) {
    fprintf(stderr, "error: %s\n", description);
}

static void cursor_position_callback(GLFWwindow *pwindow, double pxpos, double pypos) {
    xpos = pxpos;
    ypos = pypos;
}

static void mouse_button_callback(GLFWwindow *pwindow, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        clicked_xpos = xpos;
        clicked_ypos = ypos;

        selecting = true;
    }
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        release_xpos = xpos;
        release_ypos = ypos;

        selecting = false;
    }
}

static void framebuffer_resize_callback(GLFWwindow *pwindow, int pwidth, int pheight) {
    height = pheight;
    width = pwidth;

    framebufferResized = true;
}