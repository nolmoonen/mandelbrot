// nolmoonen v1.0.0
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

int32_t nm_log_init(log_level_t p_level, bool p_thread_safe)
{
    m_level = p_level;
    m_thread_safe = p_thread_safe;
    if (m_thread_safe && (pthread_mutex_init(&m_log_mutex, NULL) != 0)) {
        nm_log(LOG_ERROR, "failed to initialize protection mutex\n");
        return EXIT_FAILURE;
    }

    nm_log(LOG_INFO, "initialized nm_log\n");

    return EXIT_SUCCESS;
}

void nm_log_cleanup()
{
    if (m_thread_safe) {
        pthread_mutex_destroy(&m_log_mutex);
    }
}


void nm_log_level(log_level_t p_level)
{
    m_level = p_level;
}

void nm_log(log_level_t t_level, const char *t_format, ...)
{
    if (t_level >= m_level) {
        if (m_thread_safe) {
            pthread_mutex_lock(&m_log_mutex);
        }

        fprintf(stdout, "%s ", LEVEL_NAMES[t_level]);
        va_list argptr;
        va_start(argptr, t_format);
        vfprintf(stdout, t_format, argptr);
        va_end(argptr);

        if (m_thread_safe) {
            pthread_mutex_unlock(&m_log_mutex);
        }
    }
}