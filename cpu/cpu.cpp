#include "cpu.h"

void processorDump(FILE *fp, CPU *cpu)
{
    if (fp == nullptr)
        fp = stderr;

    if (cpu == nullptr)
        return;

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
}

#define DEF_CMD(name, num, arg, cpu_code) \
case COMMAND_CODES::CMD_##name:           \
    cpu_code                              \
    break;                                \

size_t run(CPU *cpu)
{
    assert(cpu != nullptr);

    Stack stack_run = {};
    Stack stack_call_run = {};
    size_t stackError = 0;

    if (cpu->stack == nullptr)
    {
        cpu->stack = &stack_run;
        stackCtor(cpu->stack, 1, &stackError)
    }

    if (cpu->call_stack == nullptr)
    {
        cpu->call_stack = &stack_call_run;
        stackCtor(cpu->call_stack, 1, &stackError)
    }

    size_t error = NO_ERRORS;

    while (cpu->ip < cpu->code->len)
    {
        uint8_t command = cpu->code->code[cpu->ip] & CMD_MASK;
        uint8_t args = cpu->code->code[cpu->ip] & ARG_MASK;
        switch (command)
        {
#include "../cmd.h"
            default:
            {
                fprintf(stderr, "CPU_UNKNOWN_COMMAND\n");
                return CPU_ERRORS::CPU_UNKNOWN_COMMAND;
            }
        }



    }
    return CPU_ERRORS::CPU_NO_ERRORS;
}

#undef applyOperation
#undef DEF_CMD