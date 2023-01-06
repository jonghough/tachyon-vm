#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <sys/types.h>
#include <time.h>
#include "log.h"

char *log_level_str(ty_log_level level)
{
    switch (level)
    {
    case TY_DEBUG:
        return "\033[0;36mTY DEBUG::\033[0m ";
    case TY_INFO:
        return "\033[0;33mTY INFO::\033[0m ";
    case TY_WARN:
        return "\033[0;34mTY WARN::\033[0m ";
    case TY_ERROR:
        return "\033[0;31mTY ERROR::\033[0m ";
    }
    return "";
}
void ty_log(ty_log_level level, char *msg, ...)
{

    time_t t = time(NULL);
    if (t == -1)
    {
        printf("Time not available.\n");
        exit(1);
    }
    struct tm *xtm = localtime(&t);

    printf("(%d-%02d-%02d %02d:%02d:%02d) ", xtm->tm_year + 1900, xtm->tm_mon + 1, xtm->tm_mday, xtm->tm_hour, xtm->tm_min, xtm->tm_sec);
    va_list args;
    va_start(args, msg);
    printf("%s", log_level_str(level));

    vprintf(msg, args);
    va_end(args);
}

static void _inspect_data(data *d, int depth)
{
    if (depth > 5)
    {
        ty_log(TY_ERROR, "Inspection of data item is too deep.\n");
        return;
    }
    char prefix[depth + 1];
    for (int n = 0; n < depth; n++)
    {
        prefix[n] = *"\t";
    }
    prefix[depth] = *"\0";
    if (depth == 1)
    {
        ty_log(TY_WARN, "%s~~~~~~~INSPECTING DATA~~~~~~~~\n", prefix);
    }
    ty_log(TY_WARN, "%sdata type: %d,\tis marked: %hhd\n", prefix, d->type, d->marked);
    switch (d->type)
    {
    case NONE:
        break;
    case INT8:
        ty_log(TY_WARN, "%s└─%hhd\n", prefix, d->content.xi8);
        break;
    case INT32:
        ty_log(TY_WARN, "%s└─%d\n", prefix, d->content.xi32);
        break;
    case INT64:
        ty_log(TY_WARN, "%s└─%ld\n", prefix, d->content.xi64);
        break;
    case FLT32:
        ty_log(TY_WARN, "%s└─%f\n", prefix, d->content.xf32);
        break;
    case FLT64:
        ty_log(TY_WARN, "%s└─%lf\n", prefix, d->content.xf64);
        break;
    case PTR:
        ty_log(TY_WARN, "%s└─Array of length %d\n", prefix, d->content.xptr->len);
        for (uint32_t j = 0; j < d->content.xptr->len; j++)
        {
            _inspect_data(&d->content.xptr->objs[j], depth + 1);
        }
        break;
    case STR:
        ty_log(TY_WARN, "%s└─String value: %s\n", prefix, (char *)d->content.xstr->obj);
        break;
    case REC:
        ty_log(TY_WARN, "%s└─Record of length %d\n", prefix, d->content.xrec->len);
        for (uint32_t j = 0; j < d->content.xrec->len; j++)
        {
            _inspect_data(&d->content.xrec->objs[j], depth + 1);
        }
        break;
    case FILEPTR:
        ty_log(TY_WARN, "%s└─File pointer value: %d\n", prefix, (char *)d->content.xfileptr);
        break;
    }
}

void inspect_data(data *d)
{
    _inspect_data(d, 1);
}
