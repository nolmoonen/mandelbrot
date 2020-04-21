// Nol Moonen v1.0.0
#ifndef NM_LOG_H
#define NM_LOG_H

typedef enum {
    LOG_TRACE = 0,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} Level;

extern const char *LEVEL_NAMES[];

extern Level m_level;

void nm_log_level(Level t_level);

void nm_log(Level t_level, const char *t_format, ...);

#endif //NM_LOG_H
