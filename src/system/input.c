// nolmoonen v1.0.0
#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <util/log.h>
#include <string.h>

#include "input.h"

int init_input()
{
    /** allocate memory */
    m_pressed = malloc(VECTOR_COUNT * sizeof(uint32_t));
    m_down = malloc(VECTOR_COUNT * sizeof(uint32_t));
    m_released = malloc(VECTOR_COUNT * sizeof(uint32_t));

    /** set input that require an initialization */
    m_left_pressed = false;
    m_left_released = false;
    m_middle_pressed = false;
    m_middle_released = false;
    m_right_pressed = false;
    m_right_released = false;

    m_resized = false;

    return EXIT_SUCCESS;
}

void cleanup_input()
{
    free(m_released);
    free(m_down);
    free(m_pressed);
}

int pull_input()
{
    /** reset inputs of which the callback is not triggered every event poll call */
    xoffset = 0;
    yoffset = 0;

    offset_xpos = 0;
    offset_ypos = 0;

    m_left_pressed = false;
    m_left_released = false;
    m_middle_pressed = false;
    m_middle_released = false;
    m_right_pressed = false;
    m_right_released = false;

    // pressed/released events are reset, down events are maintained
    memset(m_pressed, 0, sizeof(uint32_t) * VECTOR_COUNT);
    memset(m_released, 0, sizeof(uint32_t) * VECTOR_COUNT);

    m_resized = false;

    glfwPollEvents();

    return EXIT_SUCCESS;
}

/**
 * scroll offset
 */
int set_scroll_offset(double t_xoffset, double t_yoffset)
{
    xoffset = t_xoffset;
    yoffset = t_yoffset;

    return EXIT_SUCCESS;
}

double get_xoffset()
{
    return xoffset;
}

double get_yoffset()
{
    return yoffset;
}

/**
 * mouse inputs
 */
void set_left_pressed(bool p_left_pressed)
{ m_left_pressed = p_left_pressed; }

void set_left_released(bool p_left_released)
{ m_left_released = p_left_released; }

void set_middle_pressed(bool p_middle_pressed)
{ m_middle_pressed = p_middle_pressed; }

void set_middle_released(bool p_middle_released)
{ m_middle_released = p_middle_released; }

void set_right_pressed(bool p_right_pressed)
{ m_right_pressed = p_right_pressed; }

void set_right_released(bool p_right_released)
{ m_right_released = p_right_released; }

bool is_left_pressed()
{ return m_left_pressed; }

bool is_left_released()
{ return m_left_released; }

bool is_middle_pressed()
{ return m_middle_pressed; }

bool is_middle_released()
{ return m_middle_released; }

bool is_right_pressed()
{ return m_right_pressed; }

bool is_right_released()
{ return m_right_released; }

/**
 * cursor position
 */
int set_cursor_position(double t_xpos, double t_ypos)
{
    xpos = t_xpos;
    ypos = t_ypos;

    offset_xpos = t_xpos - last_xpos;
    offset_ypos = t_ypos - last_ypos;

    last_xpos = t_xpos;
    last_ypos = t_ypos;

    return EXIT_SUCCESS;
}

double get_xpos()
{
    return xpos;
}

double get_ypos()
{
    return ypos;
}

double get_offset_xpos()
{
    return offset_xpos;
}

double get_offset_ypos()
{
    return offset_ypos;
}

/**
 * keyboard
 */
uint32_t *get_vector_from_state(key_state_t p_state)
{
    switch (p_state) {
        case PRESSED:
            return m_pressed;
        case RELEASED:
            return m_released;
        case DOWN:
            return m_down;
        default:
            nm_log(LOG_ERROR, "unknown key state %d\n", p_state);
            return NULL;
    }
}

void set_key_state(key_value_t p_value, key_state_t p_state)
{
    if (p_value >= KEY_COUNT) {
        nm_log(LOG_WARN, "unknown key value %d\n", p_value);
        return;
    }

    uint32_t *vector = get_vector_from_state(p_state);
    vector[p_value / BITS_PER_MASK] |= 1u << (p_value % BITS_PER_MASK);
}

void unset_key_state(key_value_t p_value, key_state_t p_state)
{
    if (p_value >= KEY_COUNT) {
        nm_log(LOG_WARN, "unknown key value %d\n", p_value);
        return;
    }

    uint32_t *vector = get_vector_from_state(p_state);
    vector[p_value / BITS_PER_MASK] &= ~(1u << (p_value % BITS_PER_MASK));
}

bool get_key_state(key_value_t p_value, key_state_t p_state)
{
    if (p_value >= KEY_COUNT) {
        nm_log(LOG_WARN, "unknown key value %d\n", p_value);
        return false;
    }

    uint32_t *vector = get_vector_from_state(p_state);
    uint32_t mask = 1u << (p_value % BITS_PER_MASK);
    return vector[p_value / BITS_PER_MASK] & mask;
}

/**
 * framebuffer
 */
void set_resized(bool p_resized)
{
    m_resized = p_resized;
}

void set_iconified(bool p_iconified)
{
    m_iconified = p_iconified;
}

bool is_resized()
{
    return m_resized;
}

bool is_iconified()
{
    return m_iconified;
}