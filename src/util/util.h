// nolmoonen v1.0.0
#ifndef NM_UTIL_H
#define NM_UTIL_H

#include <stdint.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
    uint64_t m_size;    // amount of timestamps stored
    uint64_t m_dt;      // period in seconds over which to return timestamps
    clock_t *m_array;   // array of timestamps in seconds
    uint64_t m_pointer; // index of next timestamp to add
} circular_tick_buffer_t;

/**
 * Creates a buffer.
 * Every {@param t_buffer} should be deleted with a call to {@code delete_tick_buffer}. */
int create_tick_buffer(circular_tick_buffer_t *t_buffer, uint64_t t_size, uint64_t t_dt);

/** Adds a timestamp to the array. */
int tick_buffer_add(circular_tick_buffer_t *t_buffer, clock_t t_value);

/** Returns the amount of timestamps within dt distance from value. */
uint64_t tick_buffer_query(circular_tick_buffer_t *t_buffer, clock_t t_value);

/** Deletes a buffer. */
int delete_tick_buffer(circular_tick_buffer_t *t_buffer);

/** Returns the number of characters in a file. */
int file_size(uint32_t *t_size, const char *t_file_path);

/**
 * Reads a file into a buffer. Provide the number of characters in the file.
 * Assumes {@param t_file_size} + 1 bytes have been allocated to {@param t_buffer}. */
int read_to_buffer(char *t_buffer, const char *t_file_path, uint32_t t_file_size);

#endif //NM_UTIL_H
