#include <stdlib.h>
#include <GLFW/glfw3.h>
#include <stdio.h>

#include "input.h"

int init_input()
{
    /** set input that require an initialization */
    m_left_pressed = false;
    m_left_released = false;
    m_middle_pressed = false;
    m_middle_released = false;
    m_right_pressed = false;
    m_right_released = false;

    // reset keyboard inputs
    w_down = false;
    a_down = false;
    s_down = false;
    d_down = false;
    esc_down = false;
    bs_down = false;
    bs_up = false;

    m_resized = false;

    return EXIT_SUCCESS;
}

int cleanup_input()
{
    return EXIT_SUCCESS;
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

    // "up"-events need to be reset
    bs_up = false;

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
int set_w_down(bool t_down)
{
    w_down = t_down;
    return EXIT_SUCCESS;
}

int set_a_down(bool t_down)
{
    a_down = t_down;
    return EXIT_SUCCESS;
}

int set_s_down(bool t_down)
{
    s_down = t_down;
    return EXIT_SUCCESS;
}

int set_d_down(bool t_down)
{
    d_down = t_down;
    return EXIT_SUCCESS;
}

void set_esc_down(bool p_down)
{
    esc_down = p_down;
}

void set_bs_down(bool p_down)
{
    bs_down = p_down;
}

void set_bs_up(bool p_up)
{
    bs_up = p_up;
}

bool is_w_down()
{
    return w_down;
}

bool is_a_down()
{
    return a_down;
}

bool is_s_down()
{
    return s_down;
}

bool is_d_down()
{
    return d_down;
}

bool is_esc_down()
{
    return esc_down;
}

bool is_bs_down()
{
    return bs_down;
}

bool is_bs_up()
{
    return bs_up;
}

void set_resized(bool p_resized)
{
    m_resized = p_resized;
}

bool is_resized()
{
    return m_resized;
}