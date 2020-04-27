// nolmoonen v1.0.0
#ifndef NM_LOG_H
#define NM_LOG_H

#include <stdbool.h>
#include <stdint.h>
#include <pthread.h>

typedef enum {
    LOG_TRACE = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

static const char *LEVEL_NAMES[] = {
        "TRACE", "INFO ", "WARN ", "ERROR"
};

log_level_t m_level;
bool m_thread_safe;
pthread_mutex_t m_log_mutex;

/**
 * Call to {nm_log_cleanup} is required if {EXIT_SUCCESS} is returned.
 * Pass {p_thread_safe} true if thread safety is required. */
int32_t nm_log_init(log_level_t p_level, bool p_thread_safe);

void nm_log_cleanup();

/** Set the log level. */
void nm_log_level(log_level_t p_level);

/** Log with a specified level. */
void nm_log(log_level_t t_level, const char *t_format, ...);

#endif //NM_LOG_H
