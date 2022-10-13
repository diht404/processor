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

#define applyOperation(cmd_case, operation)                   \
(command == (cmd_case))                                       \
    {                                                         \
        int firstValue = 0;                                   \
        int secondValue = 0;                                  \
                                                              \
        error = stackPop(cpu->stack, &secondValue);           \
        if (error)                                            \
            return error;                                     \
                                                              \
        error = stackPop(cpu->stack, &firstValue);            \
        if (error)                                            \
            return error;                                     \
                                                              \
        if (#operation[0] == '/' and secondValue == 0)        \
            return DIVISION_BY_ZER0;                          \
        error |= stackPush(cpu->stack,                        \
                           firstValue operation secondValue); \
        if (error)                                            \
            return error;                                     \
                                                              \
        cpu->ip++;                                            \
    }

#define DEF_CMD(name, num, arg, cpu_code) \
case COMMAND_CODES::CMD_##name:           \
    cpu_code                              \
    break;                                \


size_t run(CPU *cpu)
{
    assert(cpu != nullptr);

    Stack stack_run = {};
    size_t stackError = 0;

    if (cpu->stack == nullptr)
    {
        cpu->stack = &stack_run;
        stackCtor(cpu->stack, 1, &stackError)
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