#ifndef NM_INPUT_H
#define NM_INPUT_H

#include <stdbool.h>

/** Call to {@link cleanup_input} is required if EXIT_SUCCESS is returned. */
int init_input();

int cleanup_input();

int pull_input();

/** scroll offset */
// the offset produced by scrolling
double xoffset;
double yoffset;

int set_scroll_offset(double t_xoffset, double t_yoffset);

double get_xoffset();

double get_yoffset();

/** mouse inputs */
// whether this button was pressed or released
bool m_left_pressed;
bool m_left_released;
bool m_middle_pressed;
bool m_middle_released;
bool m_right_pressed;
bool m_right_released;

void set_left_pressed(bool p_left_pressed);

void set_left_released(bool p_left_released);

void set_middle_pressed(bool p_middle_pressed);

void set_middle_released(bool p_middle_released);

void set_right_pressed(bool p_right_pressed);

void set_right_released(bool p_right_released);

bool is_left_pressed();

bool is_left_released();

bool is_middle_pressed();

bool is_middle_released();

bool is_right_pressed();

bool is_right_released();

/** cursor position */
// the position of the cursor in the last tick
double xpos;
double ypos;
// the position of the cursor in the second to last tick
double last_xpos;
double last_ypos;
// the offset produced by the cursor in the last tick
double offset_xpos;
double offset_ypos;

int set_cursor_position(double t_xpos, double t_ypos);

double get_xpos();

double get_ypos();

double get_offset_xpos();

double get_offset_ypos();

/** keyboard */
bool w_down;
bool a_down;
bool s_down;
bool d_down;
bool esc_down; // escape
// backspace
bool bs_down;
bool bs_up;

int set_w_down(bool t_down);

int set_a_down(bool t_down);

int set_s_down(bool t_down);

int set_d_down(bool t_down);

void set_esc_down(bool p_down);

void set_bs_down(bool p_down);

void set_bs_up(bool p_up);

bool is_w_down();

bool is_a_down();

bool is_s_down();

bool is_d_down();

bool is_esc_down();

bool is_bs_down();

bool is_bs_up();

/** framebuffer */
bool m_resized;

void set_resized(bool p_resized);

bool is_resized();

#endif //NM_INPUT_H
