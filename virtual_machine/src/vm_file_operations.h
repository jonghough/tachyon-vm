#ifndef VM_FILE_OPERATIONS
#define VM_FILE_OPERATIONS
#include <string.h>
#include "topcode.h"

// file ops
void fopenx(virtual_machine *v);
void freadx(virtual_machine *v);
void fwritex(virtual_machine *v);
void fclosex(virtual_machine *v);
void fdeletex(virtual_machine *v);
void fseekx(virtual_machine *v);
#endif
