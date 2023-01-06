#include <zip.h>
#include <stdio.h>
#include <stdlib.h>
#include "log.h"

char *read_program_file(const char *archive_name, const char *program_name, uint32_t *program_length)
{

    int err = 0;
    zip_t *z = zip_open(archive_name, 0, &err);
    if (err > 0)
    {
        ty_log(TY_ERROR, "Error opening zip file %s, error: %d\n", archive_name, err);
        exit(1);
    }

    struct zip_stat st;
    zip_stat_init(&st);
    zip_stat(z, program_name, 0, &st);
    char *contents = malloc((st.size + 7) * sizeof(char));
    zip_file_t *f = zip_fopen(z, program_name, 0);
    (*program_length) = (uint32_t)st.size;
    zip_fread(f, contents, st.size);
    zip_fclose(f);
    zip_close(z);
    return contents;
}
