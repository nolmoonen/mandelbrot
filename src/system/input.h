#ifndef BOMBER_INPUT_H
#define BOMBER_INPUT_H

#include <stdbool.h>

/** Call to {@link cleanup_input} is required if EXIT_SUCCESS is returned. */
int init_input();

int cleanup_input();

int pull_input();

/** scroll offset */
double xoffset;
double yoffset;

int set_scroll_offset(double t_xoffset, double t_yoffset);

double get_xoffset();

double get_yoffset();

/** mouse inputs */
bool left_pressed;
bool middle_pressed;
bool right_pressed;

bool is_left_pressed();

bool is_middle_pressed();

bool is_right_pressed();

/** cursor position */
double xpos;
double ypos;

double last_xpos;
double last_ypos;

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
bool bs_down; // backspace

int set_w_down(bool t_down);

int set_a_down(bool t_down);

int set_s_down(bool t_down);

int set_d_down(bool t_down);

void set_esc_down(bool p_down);

void set_bs_down(bool p_down);

bool is_w_down();

bool is_a_down();

bool is_s_down();

bool is_d_down();

bool is_esc_down();

bool is_bs_down();

#endif //BOMBER_INPUT_H
