#ifndef STACK__CPU_H
#define STACK__CPU_H

#include "stack/stack.h"

enum CPU_ERRORS
{
    CPU_NO_ERRORS           = 0,
    CPU_NOT_EXECUTABLE_FILE = 1 << 1,
    CPU_INCORRECT_VERSION   = 1 << 2,
    CPU_READ_FAILED         = 1 << 3,
    CPU_UNKNOWN_COMMAND     = 1 << 4,
};

struct Code
{
    int *code;
    size_t len;
};

/**
 * @brief Reads code from binary file
 *
 * @param fp file to read from
 * @param code struct to read code
 * @return
 */
size_t readCode(FILE *fp, Code *code);

/**
 * @brief Applies operators +, -, *, /
 *
 * @param stack stack for program
 * @param operation operation to execute(+, -, *, /)
 * @param error error code
 */
void applyOperator(Stack *stack, char operation, size_t *error);

/**
 * @brief runs code
 *
 * @param code code to run
 * @param stack stack for code
 * @return
 */
size_t run(Code *code, Stack *stack);

#endif //STACK__CPU_H
