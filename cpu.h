#ifndef STACK__CPU_H
#define STACK__CPU_H

#include "stack/stack.h"
#include "utils.h"

enum CPU_ERRORS
{
    CPU_NO_ERRORS                = 0,
    CPU_READ_FROM_CONSOLE_FAILED = 1 << 3,
    CPU_UNKNOWN_COMMAND          = 1 << 4,
};

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
