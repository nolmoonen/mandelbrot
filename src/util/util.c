// Nol Moonen v1.0.0
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

#include "util.h"
#include "log.h"

int create_tick_buffer(circular_tick_buffer_t *t_buffer, uint64_t t_size, uint64_t t_dt) {
    t_buffer->m_size = t_size;
    t_buffer->m_dt = t_dt;
    t_buffer->m_array = malloc(t_buffer->m_size * sizeof(clock_t));

    if (t_buffer->m_array == NULL) {
        nm_log(LOG_ERROR, "could not allocate memory for tick buffer\n");

        return EXIT_FAILURE;
    }

    t_buffer->m_pointer = 0;

    return EXIT_SUCCESS;
}

int tick_buffer_add(circular_tick_buffer_t *t_buffer, clock_t t_value) {
    t_buffer->m_array[t_buffer->m_pointer++] = t_value;
    t_buffer->m_pointer %= t_buffer->m_size;

    return EXIT_SUCCESS;
}

uint64_t tick_buffer_query(circular_tick_buffer_t *t_buffer, clock_t t_value) {
    uint64_t pointer_clone = t_buffer->m_pointer;
    uint64_t count = (uint64_t) -1;

    do {
        // set pointer to last added item
        pointer_clone = (pointer_clone + t_buffer->m_size - 1) % t_buffer->m_size;
        count++;
    } while (count != t_buffer->m_size && t_value - t_buffer->m_array[pointer_clone] <= t_buffer->m_dt);

    return count;
}

int delete_tick_buffer(circular_tick_buffer_t *t_buffer) {
    free(t_buffer->m_array);

    return EXIT_SUCCESS;
}

// fixme not tested yet
int file_size(uint32_t *t_size, const char *t_file_path) {
    FILE *fp;

    if ((fp = fopen(t_file_path, "r")) == NULL) {
        nm_log(LOG_ERROR, "failed to open file %s\n", t_file_path);

        return EXIT_FAILURE;
    }

    // go to end of file
    fseek(fp, 0, SEEK_END);

    // check how many bytes passed
    *t_size = ftell(fp);

    fclose(fp);

    return EXIT_SUCCESS;
}

// fixme not tested yet
int read_to_buffer(char *t_buffer, const char *t_file_path, uint32_t t_file_size) {
    FILE *fp;

    if ((fp = fopen(t_file_path, "r")) == NULL) {
        nm_log(LOG_ERROR, "failed to open file %s\n", t_file_path);

        return EXIT_FAILURE;
    }

    // read file into buffer
    fread(t_buffer, t_file_size, 1, fp); /* Read 1 chunk of size bytes from fp into buffer */
    t_buffer[t_file_size] = '\0';

    fclose(fp);

    return EXIT_SUCCESS;
}
