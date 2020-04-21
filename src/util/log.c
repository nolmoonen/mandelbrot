// Nol Moonen v1.0.0
#include <stdarg.h>
#include <stdio.h>

#include "log.h"

const char *LEVEL_NAMES[] = {
        "TRACE", "INFO ", "WARN ", "ERROR"
};

Level m_level = 0;

void nm_log_level(Level t_level) {
    m_level = t_level;
}

void nm_log(Level t_level, const char *t_format, ...) {
    if (t_level >= m_level) {
        fprintf(stdout, "%s ", LEVEL_NAMES[t_level]);
        va_list argptr;
        va_start(argptr, t_format);
        vfprintf(stdout, t_format, argptr);
        va_end(argptr);
    }
}