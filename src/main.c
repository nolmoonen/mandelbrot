#define STB_IMAGE_WRITE_IMPLEMENTATION

#include <stb_image_write.h>
#include <util/log.h>
#include <glad/glad.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <stdbool.h>
#include <system/input.h>
#include <system/window.h>
#include <system/shader_manager.h>
#include <util/util.h>
#include <util/mandelbrot.h>

// initial value of max_iterations
#define INITIAL_MAX_ITER 60

// amount of iterations the maximum number of iterations in the mandelbrot function is increased each step
#define ITER_STEP 20

// limit to the amount of times the fractal can be zoomed into
#define MAX_LEVELS 32

// resolution of the fractal defined by the FRACTAL_START coordinates
#define RESOLUTION (4.0f / 3.0f)

// state that is shared between the two threads
struct state {
    // limit to the number of iterations in the mandelbrot function
    uint32_t max_iterations;

    // maintained stack of fractal coordindates, to go back once zoomed
    Fractal fractal_stack[MAX_LEVELS];

    // current position on stack
    uint32_t fractal_stack_pointer;
};

quad m_quad;        // quad that is used to render the fractal and selection on
tex_t m_tex;        // texture for the fractal
tex_t m_select_tex; // texture for the selection quad

pthread_mutex_t state_mutex;          // protects {m_state}
pthread_mutex_t window_mutex;         // protects {get_window_width} and {get_window_height}
// synchronizing behavior between compute and main thread
pthread_mutex_t computing_done_mutex; // protects {texture_local}
pthread_cond_t computing_done_cv;

/** accessed by main thread only */
double clicked_xpos, clicked_ypos; // position of the mouse when selection started
bool selecting = false;            // whether something is being selected

/** accessed by both threads */
volatile bool done = false;           // program state for stopping the compute thread
volatile bool computing_done = false; // whether compute thread is done computing, and render thread may swap textures
volatile struct state m_state;        // variables related to which texture needs to be rendered
volatile Texture texture_local;       // current texture

void create_selection_matrix(mat4x4 p_selection_matrix)
{
    uint32_t w, h;
    pthread_mutex_lock(&window_mutex);
    {
        w = get_window_width();
        h = get_window_height();
    }
    pthread_mutex_unlock(&window_mutex);
    float selected_size_width = (float) (xpos - clicked_xpos);
    // height is scaled from horizontal positions to maintain window aspect ratio
    float selected_size_height = (selected_size_width / (float) w) * h;

    mat4x4_identity(p_selection_matrix);

    // translate
    mat4x4 translate;
    mat4x4_translate(
            translate,
            -1.0f + (((float) clicked_xpos * 2.0f + selected_size_width) / w),
            +1.0f - (((float) clicked_ypos * 2.0f + selected_size_height) / h),
            0.0f
    );
    mat4x4_mul(p_selection_matrix, p_selection_matrix, translate);

    // scale
    mat4x4 scale;
    mat4x4_identity(scale);
    mat4x4_scale_aniso(
            scale,
            scale,
            selected_size_width / (float) w,
            selected_size_height / (float) h,
            1.0f
    );
    mat4x4_mul(p_selection_matrix, p_selection_matrix, scale);
}

// compute thread function, non-preemptive
void *compute_function(void *vargp)
{
    while (1) {
        // if program is done, stop this thread
        if (done) break;

        // obtain data mutex and compute next texture
        pthread_mutex_lock(&computing_done_mutex);
        {
            /** obtain width and height */
            uint32_t w, h;
            pthread_mutex_lock(&window_mutex);
            {
                w = get_window_width();
                h = get_window_height();
            }
            pthread_mutex_unlock(&window_mutex);

            /** update state if expected size does not match with obtained size */
            if (texture_local.width != w || texture_local.height != h) {
                texture_local.width = w;
                texture_local.height = h;

                // change the size of the allocated buffer
                uint8_t *new_data = realloc(
                        texture_local.data, sizeof(uint8_t) * texture_local.width * texture_local.height * 4
                );
                if (new_data) {
                    texture_local.data = new_data;
                } else {
                    // todo: exit thread due to fatal error, could not reallocate memory
                    nm_log(LOG_ERROR, "cannot reallocate memory\n");
                }

                pthread_mutex_lock(&state_mutex);
                {
                    // reset max iterations
                    m_state.max_iterations = INITIAL_MAX_ITER;

                    // clear the stack and reset pointer
                    m_state.fractal_stack_pointer = 0;

                    // set the start coordinates based on new resolution
                    m_state.fractal_stack[0] = FRACTAL_START;
                    float new_res = w / (float) h;
                    if (new_res < RESOLUTION) { // new height is larger, so scale height parameters up
                        m_state.fractal_stack[0].im_start *= (RESOLUTION / new_res);
                        m_state.fractal_stack[0].im_end *= (RESOLUTION / new_res);
                    } else if (new_res > RESOLUTION) { // new width is larger, so scale width parameters up
                        m_state.fractal_stack[0].re_start *= (new_res / RESOLUTION);
                        m_state.fractal_stack[0].re_end *= (new_res / RESOLUTION);
                    } else {
                        // resolution is exactly the same, and FRACTAL_START is appropriate
                    }
                }
                pthread_mutex_unlock(&state_mutex);
            }

            nm_log(LOG_TRACE, "starting to compute for size=%ux%u\n", texture_local.width, texture_local.height);

            uint32_t maxiter, depth;
            Fractal fractal;
            /** obtain the state mutex and copy the values needed to compute the next texture */
            pthread_mutex_lock(&state_mutex);
            {
                depth = m_state.fractal_stack_pointer;
                fractal = m_state.fractal_stack[m_state.fractal_stack_pointer];
                maxiter = m_state.max_iterations;
                m_state.max_iterations += ITER_STEP;
            }
            pthread_mutex_unlock(&state_mutex);

            nm_log(LOG_TRACE, "copied fractal info for max_iter=%u, depth=%u\n", maxiter, depth);

            generate(&texture_local, fractal, maxiter);

            computing_done = true; // tell main thread that texture has been completed

            // wait until main thread has recreated texture pipeline
            pthread_cond_wait(&computing_done_cv, &computing_done_mutex);
        }
        pthread_mutex_unlock(&computing_done_mutex);
    }

    return NULL;
}

/** Updates the state of the program by using the input handler. */
void update();

/** Renders both quads. */
void render();

int main(int argc, char **argv)
{
    nm_log_init(LOG_TRACE, true);

    init_input();

    if (init_window(true) == EXIT_FAILURE) {
        nm_log(LOG_ERROR, "failed to initialize window\n");
        return EXIT_FAILURE;
    }

    // create quad
    create_quad(&m_quad);
    initialize_shader_manager();

    // create selection texture from one blue pixel
    uint8_t select_pixel[] = {51, 153, 255, 100};
    create_tex_from_mem(&m_select_tex, GL_TEXTURE0, select_pixel, 1, 1, 4);

    // thread handle for the compute thread
    pthread_t compute_thread;

    // set up state variables
    m_state.max_iterations = INITIAL_MAX_ITER;
    m_state.fractal_stack_pointer = 0;
    m_state.fractal_stack[m_state.fractal_stack_pointer] = FRACTAL_START;

    // initialize synchronization variables
    pthread_mutex_init(&state_mutex, NULL);
    pthread_mutex_init(&window_mutex, NULL);
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
        pthread_mutex_lock(&window_mutex);
        {
            pull_input();
        }
        pthread_mutex_unlock(&window_mutex);

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
    pthread_mutex_destroy(&window_mutex);

    // destroy all condition variables
    pthread_cond_destroy(&computing_done_cv);

    // join the compute thread
    pthread_join(compute_thread, NULL);

    // free allocated memory
    free(texture_local.data);

    cleanup_shader_manager();
    delete_quad(&m_quad);
    cleanup_input();
    nm_log_cleanup();
}

void render()
{
    clear_window();

    // draw the fractal quad
    mat4x4 identity;
    mat4x4_identity(identity);
    mat4x4_scale_aniso(identity, identity, 1.f, -1.f, 1.f);
    render_ortho_tex_quad(&m_quad, &m_tex, identity);

    // draw the selection quad (if selecting)
    if (selecting) {
        mat4x4 selection_matrix;
        create_selection_matrix(selection_matrix);
        render_ortho_tex_quad(&m_quad, &m_select_tex, selection_matrix);
    }
}

void update()
{
    /** replace texture */
    // if computing is done, to apply computed texture to window
    // this is done before processing input
    if (computing_done) {
        pthread_mutex_lock(&computing_done_mutex);
        {
            // replace the texture for the computed one
            delete_tex(&m_tex);
            create_tex_from_mem(&m_tex, GL_TEXTURE0, texture_local.data, texture_local.width, texture_local.height, 4);

            // signal compute thread that pipeline has been recreated
            pthread_cond_signal(&computing_done_cv);
            computing_done = false;
        }
        pthread_mutex_unlock(&computing_done_mutex);
    }

    /** update state from input*/
    // p dumps the texture to file
    if (get_key_state(KEY_P, PRESSED)) {
        // protects {texture_local}
        pthread_mutex_lock(&computing_done_mutex);
        {
            nm_log(LOG_TRACE, "dumping texture to file\n");
            const uint32_t channels = 3;
            stbi_write_png(
                    "mandelbrot.png", texture_local.width, texture_local.height, channels, texture_local.data,
                    (int32_t) (texture_local.width * channels)
            );
        }
        pthread_mutex_unlock(&computing_done_mutex);
    }

    // escape closes the window
    if (get_key_state(ESCAPE, PRESSED)) {
        set_window_to_close();
    }

    // backspace zooms out
    if (get_key_state(BACKSPACE, RELEASED)) {
        nm_log(LOG_TRACE, "zooming out\n");
        pthread_mutex_lock(&state_mutex);
        {
            if (m_state.fractal_stack_pointer == 0) {
                fprintf(stdout, "cannot go back!\n");
            } else {
                m_state.fractal_stack_pointer--;
                m_state.max_iterations = INITIAL_MAX_ITER;
            }
        }
        pthread_mutex_unlock(&state_mutex);
    }

    if (selecting) {
        if (is_right_pressed()) {
            // cancel selection (were selecting, right mouse button is pressed)
            nm_log(LOG_TRACE, "cancelled selection\n");
            selecting = false;
        } else if (is_left_released()) {
            // confirm selection (were selecting, left mouse button is released)
            nm_log(LOG_TRACE, "confirmed selection\n");
            pthread_mutex_lock(&state_mutex);
            {
                if (m_state.fractal_stack_pointer == MAX_LEVELS - 1) {
                    fprintf(stdout, "maximum depth reached!\n");
                } else {
                    if (selecting) { // to allow for canceling selection
                        uint32_t w, h;
                        pthread_mutex_lock(&window_mutex);
                        {
                            w = get_window_width();
                            h = get_window_height();
                        }
                        pthread_mutex_unlock(&window_mutex);

                        // obtain new ypos based on aspect ratio rather than real ypos
                        float ypos_scaled =
                                (float) clicked_ypos + ((float) (xpos - clicked_xpos) / w) * h;

                        volatile Fractal *curr_fractal = &m_state.fractal_stack[m_state.fractal_stack_pointer];

                        double re_size = curr_fractal->re_end - curr_fractal->re_start;
                        double im_size = curr_fractal->im_end - curr_fractal->im_start;

                        // new fractal definition
                        Fractal next_fractal = {
                                .re_start = curr_fractal->re_start + (clicked_xpos / w) * re_size,
                                .re_end =curr_fractal->re_end - ((w - xpos) / w) * re_size,
                                .im_start =curr_fractal->im_start + (clicked_ypos / h) * im_size,
                                .im_end = curr_fractal->im_end - ((h - ypos_scaled) / h) * im_size,
                        };

                        // add to next position on stack
                        m_state.fractal_stack[++m_state.fractal_stack_pointer] = next_fractal;

                        // reset the max iterations
                        m_state.max_iterations = INITIAL_MAX_ITER;
                    }
                }
            }
            pthread_mutex_unlock(&state_mutex);

            selecting = false;
        }
    } else { // if (!selecting)
        if (is_left_pressed()) {
            nm_log(LOG_TRACE, "starting to select\n");
            // start selecting
            clicked_xpos = get_xpos();
            clicked_ypos = get_ypos();

            selecting = true;
        }
    }

    // update state
    if (is_resized()) {
        nm_log(LOG_TRACE, "resized\n");
        uint32_t w, h;
        pthread_mutex_lock(&window_mutex);
        {
            w = get_window_width();
            h = get_window_height();
        }
        pthread_mutex_unlock(&window_mutex);

        glViewport(0, 0, w, h);
    }
}