#include "cpu.h"

#define DEF_CMD(name, num, arg, cpu_code) \
case COMMAND_CODES::CMD_##name:           \
    cpu_code                              \
    break;                                \

size_t run(CPU *cpu)
{
    CHECK_NULLPTR_ERROR(cpu, CPU_IS_NULLPTR)

    size_t error = NO_ERRORS;

    if (cpu->stack == nullptr)
        return STACK_NULLPTR;

    if (cpu->call_stack == nullptr)
        return STACK_NULLPTR;

    while (cpu->ip < cpu->code->len)
    {
        uint8_t command = cpu->code->code[cpu->ip] & CMD_MASK;
        uint8_t args = cpu->code->code[cpu->ip] & ARG_MASK;
        switch (command)
        {
#include "../common/cmd.h"
            default:
            {
                fprintf(stderr, "CPU_UNKNOWN_COMMAND\n");
                return CPU_ERRORS::CPU_UNKNOWN_COMMAND;
            }
        }
    }
    return CPU_ERRORS::CPU_NO_ERRORS;
}

#undef DEF_CMD

void show_ram_data(CPU *cpu, int start, int size)
{
    printf("\n");
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            printf("%s", RAM_MEM[start + size * y + x] ? "* " : ". ");
        }
        printf("\n");
    }
}

size_t processorDump(FILE *fp, CPU *cpu)
{
    if (fp == nullptr)
        fp = stderr;

    CHECK_NULLPTR_ERROR(cpu, CPU_IS_NULLPTR)

    for (size_t i = 0; i < cpu->code->len; i++)
    {
        if (cpu->ip == i)
        {
            fprintf(fp, "{{{ %d }}} ", cpu->code->code[i]);
            continue;
        }
        fprintf(fp, "%d ", cpu->code->code[i]);
    }
    fprintf(fp, "\n");
    return CPU_NO_ERRORS;
}

void handleCpuError(size_t error, CPU *cpu)
{
    FILE *fp = stderr;
    if (!error)
    {
        return;
    }
    if (error)
        processorDump(fp, cpu);

    if (error & CPU_READ_FROM_CONSOLE_FAILED)
        fprintf(fp, "Reading from console failed.\n");
    if (error & CPU_UNKNOWN_COMMAND)
        fprintf(fp, "Got unknown command.\n");
    if (error & DIVISION_BY_ZER0)
        fprintf(fp, "Division by zero.\n");
    if (error & CPU_IS_NULLPTR)
        fprintf(fp, "CPU is pullptr.\n");
}