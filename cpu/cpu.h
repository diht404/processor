#ifndef CPU__CPU_H
#define CPU__CPU_H

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
    static const int vram_size = 100*100;
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

/**
 * @brief dumps processor
 *
 * @param fp file to write dump
 * @param cpu cpu to dump
 * @return error code
 */
size_t processorDump(FILE *fp, CPU *cpu);

/**
 * @brief process error
 *
 * @param error error code
 * @param cpu cpu with error
 */
void handleCpuError(size_t error, CPU *cpu);

/**
 * @brief prints RAM data
 *
 * @param cpu cpu with RAM
 * @param start first index of picture
 * @param size size of square to show
 */
void show_ram_data(CPU *cpu, int start, int size);

#endif //CPU__CPU_H
