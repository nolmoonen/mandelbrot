// nolmoonen v1.0.0
#ifndef NM_INPUT_H
#define NM_INPUT_H

#include <stdbool.h>

/** Call to {@link cleanup_input} is required if EXIT_SUCCESS is returned. */
int init_input();

void cleanup_input();

int pull_input();

/** scroll offset */
// the offset produced by scrolling
double xoffset;
double yoffset;

int set_scroll_offset(double t_xoffset, double t_yoffset);

double get_xoffset();

double get_yoffset();

/** mouse inputs */
// todo apply same scheme as for keyboard (possibly in same masks)
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
#define KEY_COUNT 3      // number of key values defined in {key_value_t}
#define BITS_PER_MASK 32 // number of bits per vectors {m_pressed, m_released, m_down}
// number of vectors are needed to maintain all key values
#define VECTOR_COUNT (KEY_COUNT / BITS_PER_MASK + ((KEY_COUNT % BITS_PER_MASK) ? 1 : 0))
typedef enum {
    BACKSPACE = 0,
    ESCAPE,
    KEY_P
    // NB: update {KEY_COUNT}
} key_value_t;

typedef enum {
    PRESSED = 0,
    RELEASED,
    DOWN
} key_state_t;

uint32_t *m_pressed;  // whether key is pressed in last tick
uint32_t *m_released; // whether key is released in last tick
uint32_t *m_down;     // whether key is down at end of last tick

void set_key_state(key_value_t p_value, key_state_t p_state);

// really only used for {m_down} since {m_pressed} and m_released are reset in {pull_input}
void unset_key_state(key_value_t p_value, key_state_t p_state);

bool get_key_state(key_value_t p_value, key_state_t p_state);

/** framebuffer */
bool m_resized;

void set_resized(bool p_resized);

bool is_resized();

#endif //NM_INPUT_H
