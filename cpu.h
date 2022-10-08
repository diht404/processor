#ifndef STACK__CPU_H
#define STACK__CPU_H

#include "stack/stack.h"
#include "stack/stack_logs.h"
#include "utils.h"

enum CPU_ERRORS
{
    CPU_NO_ERRORS                = 0,
    CPU_READ_FROM_CONSOLE_FAILED = 1 << 3,
    CPU_UNKNOWN_COMMAND          = 1 << 4,
};

struct CPU
{
    Code *code = nullptr;
    Stack *stack = nullptr;
    size_t ip = 0;
    int regs[5] = {0, 0, 0, 0, 0};
    int RAM[100] = {0};
};

/**
 * @brief runs code
 *
 * @param code code to run
 * @param stack stack for code
 * @return
 */
size_t run(CPU *cpu);

void processorDump(FILE *fp, CPU *cpu);

#endif //STACK__CPU_H
