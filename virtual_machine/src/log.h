#ifndef LOG_H
#define LOG_H
#include "datatypes.h"
typedef enum ty_log_level
{
    TY_INFO,
    TY_DEBUG,
    TY_WARN,
    TY_ERROR
} ty_log_level;

char *log_level_str(ty_log_level level);
void ty_log(ty_log_level level,char *msg, ... );
void inspect_data(data * d);

#endif
