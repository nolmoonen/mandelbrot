#define GLFW_INCLUDE_VULKAN

#include <GLFW/glfw3.h>

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <unistd.h> // sleep

#include "math.h"
#include "vulkan.h"
#include "complex.h"
#include "color.h"

// initial value of max_iterations
#define INITIAL_MAX_ITER 60

// amount of iterations the maximum number of iterations in the mandelbrot function is increased each step
#define ITER_STEP 20

// limit to the amount of times the fractal can be zoomed into
#define MAX_LEVELS 32

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

pthread_mutex_t data_mutex; // protects all data shared between compute and window/render thread

// limit to the number of iterations in the mandelbrot function
uint32_t max_iterations;

Texture *texture_local; // current texture

Fractal fractal_stack[MAX_LEVELS]; // stack of fractals to go back once zoomed
uint32_t current_level = 0; // current position on stack

uint32_t screen_width;
uint32_t screen_height;

double xpos, ypos; // position of the mouse
double clicked_xpos, clicked_ypos; // position of the mouse pressed down

bool selecting = false; // whether something is being selected

bool done = false; // program state for stopping the compute thread
pthread_mutex_t done_mutex; // protects the done mutex

bool computing_done = false; // whether compute thread is done computing, and render thread may swap textures
pthread_mutex_t computing_done_mutex;
pthread_cond_t computing_done_cv;

pthread_mutex_t compute_idle_mutex;
pthread_cond_t compute_idle_cv;

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
    hsv.v = m < max_iterations ? 255 : 0;
    struct RGB rgb = HSVtoRGB(hsv);
    return rgb;
}

/**
 * Calculates iterations to converge, given a complex number.
 */
double mandelbrot(struct Complex c) {
    struct Complex z = {0, 0};
    uint32_t n = 0;

    while (abs_complex(z) <= 2 && n < max_iterations) {
        z = add_complex(mul_complex(z, z), c);
        ++n;
    }

    if (n == max_iterations) {
        return max_iterations;
    }

    return n + 1 - log(log2(abs_complex(z)));
}

void generate(Texture *p_texture) {
    double *all_iterations = (double *) malloc(sizeof(double) * p_texture->width * p_texture->height);
    uint32_t *histogram = calloc(max_iterations - 1, sizeof(uint32_t));
    uint32_t total = 0;

    /*
     * calculate iterations
     */
    double re_diff = fractal_stack[current_level].re_end - fractal_stack[current_level].re_start;
    double im_diff = fractal_stack[current_level].im_end - fractal_stack[current_level].im_start;
    for (uint32_t y = 0; y < p_texture->height; ++y) {
        for (uint32_t x = 0; x < p_texture->width; ++x) {
            // convert pixel coordinate to complex number
            struct Complex c = {
                    fractal_stack[current_level].re_start + (x / (double) p_texture->width) * re_diff,
                    fractal_stack[current_level].im_start + (y / (double) p_texture->height) * im_diff,
            };

            // compute number of iterations
            double m = mandelbrot(c);
            all_iterations[y * p_texture->width + x] = m;

            if (m < max_iterations) {
                histogram[(uint32_t) floor(m)]++;
                total++;
            }
        }
    }

    double *hues = (double *) malloc(sizeof(double) * (max_iterations + 1));
    double h = 0;
    for (uint32_t i = 0; i < max_iterations; ++i) {
        h += histogram[i] / (double) total;
        hues[i] = h;
    }
    hues[max_iterations] = h;

    /*
     * calculate and set colors
     */
    for (uint32_t y = 0; y < p_texture->height; ++y) {
        for (uint32_t x = 0; x < p_texture->width; ++x) {
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

UniformBufferObject calculate_uniform_buffer_object() {
    Mat4f proj = MAT4F_IDENTITY;

    if (selecting) {
        int32_t selected_width = xpos - clicked_xpos;
        // height is scaled from horizontal positions
        double scaled_selected_height = (selected_width / (double) screen_width) * screen_height;

        /*
         * translate
         */
        proj = cross(proj, translate((Vec3f) {
                -1.0f + ((clicked_xpos * 2.0f + selected_width) / (float) screen_width),
                -1.0f + ((clicked_ypos * 2.0f + scaled_selected_height) / (float) screen_height),
                0.0f,
        }));

        /*
         * scaling
         */
        proj = cross(proj, scale((Vec3f) {
                selected_width / (float) screen_width,
                scaled_selected_height / (float) screen_height,
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

// compute thread function, non-preemptive
void *compute_function(void *vargp) {
    while (1) {
        pthread_mutex_lock(&done_mutex);
        {
            if (done) {
                break;
            }
        }
        pthread_mutex_unlock(&done_mutex);

        // compute next texture
        pthread_mutex_lock(&data_mutex);
        {
            if (texture_local->width != screen_width || texture_local->height != screen_height) {
                texture_local->width = screen_width;
                texture_local->height = screen_height;
                free(texture_local->data);
                texture_local->data = (uint8_t *) malloc(sizeof(uint8_t) * screen_width * screen_height * 4);
            }
            generate(texture_local);

            max_iterations += ITER_STEP;
        }
        pthread_mutex_unlock(&data_mutex);

        pthread_mutex_lock(&computing_done_mutex);
        {
            computing_done = true; // tell main thread that texture has been completed

            // signal main thread that compute thread is idle
            pthread_mutex_lock(&compute_idle_mutex);
            {
                pthread_cond_signal(&compute_idle_cv);
            }
            pthread_mutex_unlock(&compute_idle_mutex);

            // wait until main thread has recreated texture pipeline
            pthread_cond_wait(&computing_done_cv, &computing_done_mutex);
        }
        pthread_mutex_unlock(&computing_done_mutex);
    }

    return NULL;
}

int main() {
    GLFWwindow *window; // window handle
    pthread_t compute_thread; // the id of the thread making the calculations, needed for joining

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

    pthread_mutex_init(&data_mutex, NULL);

    // set up coordinate stack
    fractal_stack[current_level] = FRACTAL_START;

    // initialize width and height
    screen_width = WIDTH;
    screen_height = HEIGHT;

    max_iterations = INITIAL_MAX_ITER;

    texture_local = (Texture *) malloc(sizeof(Texture));
    texture_local->width = screen_width;
    texture_local->height = screen_height;
    // separately allocate this memory since "data" is a pointer
    texture_local->data = (uint8_t *) malloc(sizeof(uint8_t) * screen_width * screen_height * 4);

    // initialize vulkan
    if (!vulkanInit(window, texture_local, &calculate_uniform_buffer_object)) {
        glfwTerminate();
        return 1;
    }

    // initialize synchronization variables
    pthread_mutex_init(&done_mutex, NULL);
    pthread_mutex_init(&compute_idle_mutex, NULL);
    pthread_cond_init(&compute_idle_cv, NULL);
    pthread_mutex_init(&computing_done_mutex, NULL);
    pthread_cond_init(&computing_done_cv, NULL);

    // create the compute thread
    pthread_create(&compute_thread, NULL, compute_function, NULL);

    // create circular buffer for title
    CircularTickBuffer circular_buffer;
    tick_buffer_init(&circular_buffer, 20000, CLOCKS_PER_SEC);

    // main loop
    char title[256];
    uint32_t max_iter_copy = INITIAL_MAX_ITER; // local copy of max_iterations
    uint32_t current_lvl_copy = 0; // local copy of current_level
    while (!glfwWindowShouldClose(window)) {
        tick_buffer_add(&circular_buffer, clock());
        glfwPollEvents();
        drawFrame();

        pthread_mutex_lock(&computing_done_mutex);
        {
            // if computing is done, recreate texture pipeline
            if (computing_done) {
                pthread_mutex_lock(&data_mutex);
                {
                    recreateTexture();

                    // cache variables for title. obtaining mutex in window thread would be too slow
                    max_iter_copy = max_iterations;
                    current_lvl_copy = current_level;
                }
                pthread_mutex_unlock(&data_mutex);

                // signal compute thread that pipeline has been recreated
                pthread_cond_signal(&computing_done_cv);
                computing_done = false;
            }
        }
        pthread_mutex_unlock(&computing_done_mutex);

        // create and display title
        memset(&title[0], 0, sizeof(title)); // clear all values
        // fill with actual title data
        snprintf(title, 255, "fps: %d iter: %d lvl: %d",
                 tick_buffer_query(&circular_buffer, clock()), max_iter_copy, current_lvl_copy);
        title[255] = '\0'; // add null terminator
        glfwSetWindowTitle(window, title);
    }

    // set status to done to signal compute thread to stop
    pthread_mutex_lock(&done_mutex);
    {
        done = true;
    }
    pthread_mutex_unlock(&done_mutex);

    // wait until compute thread is idle
    pthread_mutex_lock(&compute_idle_mutex);
    {
        pthread_cond_wait(&compute_idle_cv, &compute_idle_mutex);
    }
    pthread_mutex_unlock(&compute_idle_mutex);

    // signal compute thread to it stops waiting
    pthread_mutex_lock(&computing_done_mutex);
    {
        pthread_cond_signal(&computing_done_cv);
    }
    pthread_mutex_unlock(&computing_done_mutex);

    // destroy all mutices
    pthread_mutex_destroy(&done_mutex);
    pthread_mutex_destroy(&computing_done_mutex);
    pthread_mutex_destroy(&compute_idle_mutex);
    pthread_mutex_destroy(&data_mutex);

    // destroy all condition variables
    pthread_cond_destroy(&computing_done_cv);
    pthread_cond_destroy(&compute_idle_cv);

    // join the compute thread
    pthread_join(compute_thread, NULL);

    // free allocated memory
    free(texture_local);

    // wait until vulkan can be terminated
    waitDeviceIdle();

    // clean up vulkan
    vulkanTerminate();

    // clean up glfw
    glfwDestroyWindow(window);
    glfwTerminate();
}

static void key_callback(GLFWwindow *p_window, int key, int scancode, int action, int mods) {
    // escape closes the window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(p_window, GLFW_TRUE);
    }

    // backspace zooms out
    if (key == GLFW_KEY_BACKSPACE && action == GLFW_PRESS) {
        if (current_level == 0) {
            fprintf(stdout, "cannot go back!\n");
        } else {
            pthread_mutex_lock(&data_mutex);
            {
                current_level--;
                max_iterations = INITIAL_MAX_ITER;
            }
            pthread_mutex_unlock(&data_mutex);
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
                // prevent generation happening while changing coordinates
                pthread_mutex_lock(&data_mutex);
                {
                    // obtain new ypos based on aspect ratio rather than real ypos
                    double ypos_scaled = clicked_ypos + ((xpos - clicked_xpos) / (double) screen_width) * screen_height;

                    double re_diff = fractal_stack[current_level].re_end - fractal_stack[current_level].re_start;
                    double im_diff = fractal_stack[current_level].im_end - fractal_stack[current_level].im_start;

                    // new fractal definition
                    Fractal new_pos = {
                            fractal_stack[current_level].re_start + (clicked_xpos / screen_width) * re_diff,
                            fractal_stack[current_level].re_end - ((screen_width - xpos) / screen_width) * re_diff,
                            fractal_stack[current_level].im_start + (clicked_ypos / screen_height) * im_diff,
                            fractal_stack[current_level].im_end -
                            ((screen_height - ypos_scaled) / screen_height) * im_diff,
                    };

                    // add to next position on stack
                    fractal_stack[++current_level] = new_pos;

                    max_iterations = INITIAL_MAX_ITER;
                }
                pthread_mutex_unlock(&data_mutex);
            }
        }

        selecting = false;
    }
}

static void framebuffer_resize_callback(GLFWwindow *p_window, int p_width, int p_height) {
    pthread_mutex_lock(&data_mutex);
    {
        // set new dimensions
        screen_width = p_width;
        screen_height = p_height;

        // reset max iterations
        max_iterations = INITIAL_MAX_ITER;

        // let vulkan know to rebuild swap chain
        framebufferResized = true;
    }
    pthread_mutex_unlock(&data_mutex);
}