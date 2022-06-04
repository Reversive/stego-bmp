#ifndef _LOGGER_H
#define _LOGGER_H
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef enum
{
    DEBUG = 0,
    INFO,
    ERROR,
    FATAL
} LOG_LEVEL;

extern LOG_LEVEL current_level;

void set_log_level(LOG_LEVEL newLevel);

char *level_description(LOG_LEVEL level);

char *get_current_timestamp();

#define logw(level, fmt, ...)                                                                                                                          \
    {                                                                                                                                                  \
        if (level >= current_level)                                                                                                                    \
        {                                                                                                                                              \
            FILE *toPrint = (level >= ERROR) ? stderr : stdout;                                                                                        \
                                                                                                                                                       \
            if (level == DEBUG)                                                                                                                        \
                fprintf(toPrint, "%s[%s][ %s ]\033[0m\t%s:%d, ", "\033[0;36m", get_current_timestamp(), level_description(level), __FILE__, __LINE__); \
            else if (level == INFO)                                                                                                                    \
                fprintf(toPrint, "%s[%s][ %s ]\033[0m\t%s:%d, ", "\033[0;32m", get_current_timestamp(), level_description(level), __FILE__, __LINE__); \
            else                                                                                                                                       \
                fprintf(toPrint, "%s[%s][ %s ]\033[0m\t%s:%d, ", "\033[0;31m", get_current_timestamp(), level_description(level), __FILE__, __LINE__); \
            fprintf(toPrint, fmt, __VA_ARGS__);                                                                                                        \
            fprintf(toPrint, "\n");                                                                                                                    \
        }                                                                                                                                              \
        if (level == FATAL)                                                                                                                            \
            exit(1);                                                                                                                                   \
    }
#endif
