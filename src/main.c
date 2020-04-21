#include <glad/glad.h>

#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <util/log.h>
#include <system/input.h>
#include <system/window.h>
#include <system/ortho_renderer.h>
#include <system/shader_manager.h>
#include <util/util.h>
#include <util/complex.h>
#include <util/mandelbrot.h>

// initial value of max_iterations
#define INITIAL_MAX_ITER 60

// amount of iterations the maximum number of iterations in the mandelbrot function is increased each step
#define ITER_STEP 20

// limit to the amount of times the fractal can be zoomed into
#define MAX_LEVELS 32

// resolution of the fractal defined by the FRACTAL_START coordinates
#define RESOLUTION (4.0f / 3.0f)

// defines a fractal through coordinates
typedef struct Fractal {
    double re_start;
    double re_end;
    double im_start;
    double im_end;
} Fractal;

// mandelbrot variables for defined RESOLUTION
const Fractal FRACTAL_START = {
        -2, // RE_START
        1, // RE_END
        -1, // IM_START
        1, // IM_END
};

// state that is shared between the two threads
struct state {
    // limit to the number of iterations in the mandelbrot function
    uint32_t max_iterations;

    // maintained stack of fractal coordindates, to go back once zoomed
    Fractal fractal_stack[MAX_LEVELS];

    // current position on stack
    uint32_t fractal_stack_pointer;
};

quad m_quad;
tex_t m_tex;

typedef struct Texture {
    uint8_t *data;
    uint32_t width;
    uint32_t height;
} Texture;

// protects m_state
pthread_mutex_t state_mutex;
// synchronizing behavior between compute and main thread
pthread_mutex_t computing_done_mutex;
pthread_cond_t computing_done_cv;

/** accessed by main thread only */
double xpos, ypos; // position of the mouse
double clicked_xpos, clicked_ypos; // position of the mouse pressed down
bool selecting = false; // whether something is being selected

/** accessed by both threads */
volatile bool done = false; // program state for stopping the compute thread
volatile bool computing_done = false; // whether compute thread is done computing, and render thread may swap textures
volatile struct state m_state; // variables related to which texture needs to be rendered
volatile Texture texture_local; // current texture

void generate(Texture *p_texture, Fractal p_fractal, uint32_t p_max_iterations) {
    double *all_iterations = (double *) malloc(sizeof(double) * p_texture->width * p_texture->height);
    uint32_t *histogram = calloc(p_max_iterations - 1, sizeof(uint32_t));
    uint32_t total = 0;

    /*
     * calculate iterations
     */
    double re_diff = p_fractal.re_end - p_fractal.re_start;
    double im_diff = p_fractal.im_end - p_fractal.im_start;
    for (uint32_t y = 0; y < p_texture->height; ++y) {
        for (uint32_t x = 0; x < p_texture->width; ++x) {
            // convert pixel coordinate to complex number
            complex_t c = {
                    p_fractal.re_start + (x / (double) p_texture->width) * re_diff,
                    p_fractal.im_start + (y / (double) p_texture->height) * im_diff,
            };

            // compute number of iterations
            double m = mandelbrot(c, p_max_iterations);
            all_iterations[y * p_texture->width + x] = m;

            if (m < p_max_iterations) {
                histogram[(uint32_t) floor(m)]++;
                total++;
            }
        }
    }

    double *hues = (double *) malloc(sizeof(double) * (p_max_iterations + 1));
    double h = 0;
    for (uint32_t i = 0; i < p_max_iterations; ++i) {
        h += histogram[i] / (double) total;
        hues[i] = h;
    }
    hues[p_max_iterations] = h;

    /*
     * calculate and set colors
     */
    for (uint32_t y = 0; y < p_texture->height; ++y) {
        for (uint32_t x = 0; x < p_texture->width; ++x) {
            // create color, based on the number of iterations
            color_t rgb = color(all_iterations[y * p_texture->width + x], hues, p_max_iterations);

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

//UniformBufferObject calculate_uniform_buffer_object() {
//    Mat4f proj = MAT4F_IDENTITY;
//
//    // fixme for now, dont use mutex for screen dimensions
//    if (selecting) {
//        int32_t selected_width = xpos - clicked_xpos;
//        // height is scaled from horizontal positions
//        double scaled_selected_height = (selected_width / (double) m_state.screen_width) * m_state.screen_height;
//
//        // translate
//        proj = cross(proj, translate((Vec3f) {
//                -1.0f + ((clicked_xpos * 2.0f + selected_width) / (float) m_state.screen_width),
//                -1.0f + ((clicked_ypos * 2.0f + scaled_selected_height) / (float) m_state.screen_height),
//                0.0f,
//        }));
//
//        // scale
//        proj = cross(proj, scale((Vec3f) {
//                selected_width / (float) m_state.screen_width,
//                scaled_selected_height / (float) m_state.screen_height,
//                1.0f,
//        }));
//    } else {
//        // hide selected quad behind textured quad
//        proj = cross(proj, translate((Vec3f) {
//                0.0f,
//                0.0f,
//                -1.0f,
//        }));
//    }
//
//    return (UniformBufferObject) {MAT4F_IDENTITY, MAT4F_IDENTITY, proj};
//}

// compute thread function, non-preemptive
void *compute_function(void *vargp) {
    while (1) {
        // if program is done, stop this thread
        if (done) break;

        // obtain data mutex and compute next texture
        pthread_mutex_lock(&computing_done_mutex);
        {
            uint32_t texwidth, texheight, maxiter;
            Fractal fractal;
            /** obtain the state mutex and copy the values needed to compute the next texture */
            pthread_mutex_lock(&state_mutex);
            {
                texwidth = get_window_width();
                texheight = get_window_height();
                fractal = m_state.fractal_stack[m_state.fractal_stack_pointer];
                maxiter = m_state.max_iterations;
                m_state.max_iterations += ITER_STEP;
            }
            pthread_mutex_unlock(&state_mutex);

            // if the screen size has changed in the meantime, change the size of the allocated buffer
            if (texture_local.width != texwidth || texture_local.height != texheight) {
                texture_local.width = texwidth;
                texture_local.height = texheight;

                uint8_t *new_data = realloc(texture_local.data, sizeof(uint8_t) * texwidth * texheight * 4);
                if (new_data) {
                    texture_local.data = new_data;
                } else {
                    // todo: exit thread due to fatal error, could not reallocate memory
                }
            }

            generate(&texture_local, fractal, maxiter);

            computing_done = true; // tell main thread that texture has been completed

            // wait until main thread has recreated texture pipeline
            pthread_cond_wait(&computing_done_cv, &computing_done_mutex);
        }
        pthread_mutex_unlock(&computing_done_mutex);
    }

    return NULL;
}

void update();

void render();

int main(int argc, char **argv) {
    nm_log_level(LOG_TRACE);

    init_input();

    if (init_window() == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "failed to initialize window\n");
        return EXIT_FAILURE;
    }

    // create quad
    create_quad(&m_quad);
    initialize_shader_manager();

    // thread handle for the compute thread
    pthread_t compute_thread;

    // set up state variables
    m_state.max_iterations = INITIAL_MAX_ITER;
    m_state.fractal_stack_pointer = 0;
    m_state.fractal_stack[m_state.fractal_stack_pointer] = FRACTAL_START;

    // initialize synchronization variables
    pthread_mutex_init(&state_mutex, NULL);
    pthread_mutex_init(&computing_done_mutex, NULL);
    pthread_cond_init(&computing_done_cv, NULL);

    // allocate texture
    texture_local.width = get_window_width();
    texture_local.height = get_window_height();
    texture_local.data = malloc(sizeof(uint8_t) * get_window_width() * get_window_height() * 4);

    // create the compute thread
    pthread_create(&compute_thread, NULL, compute_function, NULL);

    // initialize tick buffer for title
    circular_tick_buffer_t tick_buffer;
    create_tick_buffer(&tick_buffer, 512, CLOCKS_PER_SEC);

    // main loop
    char title[256];
    while (!window_should_close()) {
        // process events; fill input handler (through glfw callbacks)
        pull_input();

        update();

        render();

        // update window title
        // todo this block should move somewhere else (some kind of debug class?)
        tick_buffer_add(&tick_buffer, clock());
        snprintf(title, 255, "fps: %" PRId64, tick_buffer_query(&tick_buffer, clock()));
        set_window_title(title);

        // swap buffers
        swap_window_buffers();
    }
    delete_tick_buffer(&tick_buffer);

    // already cleanup window
    cleanup_window();

    // set status to done to signal compute thread to stop
    done = true;

    // signal compute thread to it stops waiting
    pthread_mutex_lock(&computing_done_mutex);
    {
        pthread_cond_signal(&computing_done_cv);
    }
    pthread_mutex_unlock(&computing_done_mutex);

    // destroy all mutices
    pthread_mutex_destroy(&computing_done_mutex);
    pthread_mutex_destroy(&state_mutex);

    // destroy all condition variables
    pthread_cond_destroy(&computing_done_cv);

    // join the compute thread
    pthread_join(compute_thread, NULL);

    // free allocated memory
    free(texture_local.data);

    delete_quad(&m_quad);

    cleanup_input();
}

void render() {
    screen_clear();

    mat4x4 identity;
    mat4x4_identity(identity);
    render_ortho_tex_quad(&m_quad, &m_tex, identity);
}

void update () {
    // if computing is done, recreate texture pipeline to apply computed texture to window
    if (computing_done) {
        pthread_mutex_lock(&computing_done_mutex);
        {
            // replace the texture for the computed one
            nm_log(LOG_INFO, "replacing texture\n");
            delete_tex(&m_tex);
            create_tex_from_buffer(&m_tex, texture_local.data, texture_local.width, texture_local.height, GL_TEXTURE0, 4);

            // signal compute thread that pipeline has been recreated
            pthread_cond_signal(&computing_done_cv);
            computing_done = false;
        }
        pthread_mutex_unlock(&computing_done_mutex);
    }

    // todo update state from input
}

//static void key_callback(GLFWwindow *p_window, int p_key, int p_scancode, int p_action, int p_mods) {
//    // escape closes the window
//    if (p_key == GLFW_KEY_ESCAPE && p_action == GLFW_PRESS) {
//        glfwSetWindowShouldClose(p_window, GLFW_TRUE);
//    }
//
//    // backspace zooms out
//    if (p_key == GLFW_KEY_BACKSPACE && p_action == GLFW_PRESS) {
//        pthread_mutex_lock(&state_mutex);
//        {
//            if (m_state.fractal_stack_pointer == 0) {
//                fprintf(stdout, "cannot go back!\n");
//            } else {
//                m_state.fractal_stack_pointer--;
//                m_state.max_iterations = INITIAL_MAX_ITER;
//            }
//        }
//        pthread_mutex_unlock(&state_mutex);
//    }
//}
//
//static void error_callback(int p_error, const char *p_description) {
//    fprintf(stderr, "error: %s\n", p_description);
//}
//
//static void cursor_position_callback(GLFWwindow *p_window, double p_xpos, double p_ypos) {
//    xpos = p_xpos;
//    ypos = p_ypos;
//}
//
//static void mouse_button_callback(GLFWwindow *p_window, int p_button, int p_action, int p_mods) {
//    // cancel selection
//    if (p_button == GLFW_MOUSE_BUTTON_RIGHT && p_action == GLFW_PRESS) {
//        if (selecting) {
//            selecting = !selecting;
//        }
//    }
//
//    // start selecting
//    if (p_button == GLFW_MOUSE_BUTTON_LEFT && p_action == GLFW_PRESS) {
//        clicked_xpos = xpos;
//        clicked_ypos = ypos;
//
//        selecting = true;
//    }
//
//    // confirm selection
//    if (p_button == GLFW_MOUSE_BUTTON_LEFT && p_action == GLFW_RELEASE) {
//        pthread_mutex_lock(&state_mutex);
//        {
//            if (m_state.fractal_stack_pointer == MAX_LEVELS - 1) {
//                fprintf(stdout, "maximum depth reached!\n");
//            } else {
//                if (selecting) { // to allow for canceling selection
//                    // obtain new ypos based on aspect ratio rather than real ypos
//                    double ypos_scaled = clicked_ypos + ((xpos - clicked_xpos) / (double) m_state.screen_width) *
//                                                        m_state.screen_height;
//
//                    Fractal *curr_fractal = &m_state.fractal_stack[m_state.fractal_stack_pointer];
//
//                    double re_diff = curr_fractal->re_end - curr_fractal->re_start;
//                    double im_diff = curr_fractal->im_end - curr_fractal->im_start;
//                    uint32_t w = m_state.screen_width;
//                    uint32_t h = m_state.screen_height;
//
//                    // new fractal definition
//                    Fractal next_fractal = {
//                            .re_start = curr_fractal->re_start + (clicked_xpos / w) * re_diff,
//                            .re_end =curr_fractal->re_end - ((w - xpos) / w) * re_diff,
//                            .im_start =curr_fractal->im_start + (clicked_ypos / h) * im_diff,
//                            .im_end = curr_fractal->im_end - ((h - ypos_scaled) / h) * im_diff,
//                    };
//
//                    // add to next position on stack
//                    m_state.fractal_stack[++m_state.fractal_stack_pointer] = next_fractal;
//
//                    // reset the max iterations
//                    m_state.max_iterations = INITIAL_MAX_ITER;
//                }
//            }
//        }
//        pthread_mutex_unlock(&state_mutex);
//
//        selecting = false;
//    }
//}
//
//static void framebuffer_resize_callback(GLFWwindow *p_window, int p_width, int p_height) {
//    // update state
//    pthread_mutex_lock(&state_mutex);
//    {
//        // set new dimensions
//        m_state.screen_width = p_width;
//        m_state.screen_height = p_height;
//
//        // reset max iterations
//        m_state.max_iterations = INITIAL_MAX_ITER;
//
//        // clear the stack and reset pointer
//        m_state.fractal_stack_pointer = 0;
//
//        // set the start coordinates based on new resolution
//        m_state.fractal_stack[0] = FRACTAL_START;
//        double new_res = p_width / (double) p_height;
//        if (new_res < RESOLUTION) { // new height is larger, so scale height parameters up
//            m_state.fractal_stack[0].im_start *= (RESOLUTION / new_res);
//            m_state.fractal_stack[0].im_end *= (RESOLUTION / new_res);
//        } else if (new_res > RESOLUTION) { // new width is larger, so scale width parameters up
//            m_state.fractal_stack[0].re_start *= (new_res / RESOLUTION);
//            m_state.fractal_stack[0].re_end *= (new_res / RESOLUTION);
//        } else {
//            // resolution is exactly the same, and FRACTAL_START is appropriate
//        }
//    }
//    pthread_mutex_unlock(&state_mutex);
//
//    // let vulkan know to rebuild swap chain
//    framebufferResized = true;
//}