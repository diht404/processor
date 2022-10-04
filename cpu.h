#ifndef STACK__CPU_H
#define STACK__CPU_H

#include "stack/stack.h"

enum CPU_ERRORS
{
    CPU_NO_ERRORS = 0,
    CPU_NOT_EXECUTABLE_FILE = 1,
    CPU_INCORRECT_VERSION = 2,
    CPU_READ_FAILED = 3,
    CPU_UNKNOWN_COMMAND = 4,
};

struct Code
{
    int *code;
    size_t len;
};

size_t readCode(FILE *fp, Code *code);

size_t run(Code *code, Stack *stack);

#endif //STACK__CPU_H
