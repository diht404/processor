#ifndef STACK__CPU_H
#define STACK__CPU_H

#include "../stack/stack.h"
#include "../stack/stack_logs.h"
#include "../common/utils.h"

enum CPU_ERRORS
{
    CPU_NO_ERRORS                = 0,
    CPU_READ_FROM_CONSOLE_FAILED = 1 << 3,
    CPU_UNKNOWN_COMMAND          = 1 << 4,
    DIVISION_BY_ZER0             = 1 << 5,
    CPU_IS_NULLPTR               = 1 << 6,
};

struct CPU
{
    Code *code = nullptr;
    Stack *stack = nullptr;
    Stack *call_stack = nullptr;
    size_t ip = 0;
    int regs[5] = {0, 0, 0, 0, 0};
    static const int vram_size = 20*20;
    int RAM[vram_size] = {0};
};

/**
 * @brief runs code
 *
 * @param code code to run
 * @param stack stack for code
 * @return
 */
size_t run(CPU *cpu);

size_t processorDump(FILE *fp, CPU *cpu);

/**
 * @brief process error
 *
 * @param error error code
 * @param cpu cpu with error
 */
void processCpuError(size_t error, CPU *cpu);

void show_ram_data(CPU *cpu, int size);

#endif //STACK__CPU_H
