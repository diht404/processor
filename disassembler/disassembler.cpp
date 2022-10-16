#include "disassembler.h"

size_t printArg(Code *code, FILE *fp, const char *command_name, size_t *ip)
{
    if (code == nullptr)
    {
        return CODE_IS_NULLPTR;
    }

    if (fp == nullptr)
    {
        return FILE_IS_NULLPTR;
    }

    if (command_name == nullptr)
    {
        return COMMAND_NAME_IS_NULLPTR;
    }

    if (ip == nullptr)
    {
        return IP_IS_NULLPTR;
    }

    uint8_t cmd = code->code[*ip];
    (*ip)++;
    int value = *(int *) (code->code + *ip);

    if (cmd & ARG_MASK)
    {
        if (cmd & REG_MASK)
        {
            if (value > 0 and value < 5)
                fprintf(fp, "%s %s\n", command_name, REGS_NAMES[value]);
            else
                return UNKNOWN_REG;
        }
        if (cmd & IMM_MASK)
        {
            if (cmd & RAM_MASK)
                fprintf(fp, "%s [%d]\n", command_name, value);
            else
                fprintf(fp, "%s %d\n", command_name, value);
        }
    }
    *ip += sizeof(int);
    return NO_ERRORS;
}

#define DEF_CMD(name, num, arg, cpu_code)\
case COMMAND_CODES::CMD_##name:          \
{                                        \
    if (arg)                             \
    {                                    \
        printArg(code, fp, #name, &ip);  \
        break;                           \
    }                                    \
    fprintf(fp, #name"\n");              \
    ip++;                                \
    break;                               \
}

size_t disassemle(Code *code, FILE *fp)
{
    if (code == nullptr)
    {
        return CODE_IS_NULLPTR;
    }

    if (fp == nullptr)
    {
        return FILE_IS_NULLPTR;
    }

    size_t ip = 0;
    while (ip < code->len)
    {
        switch (code->code[ip] & CMD_MASK)
        {
#include "../common/cmd.h"
            default:
            {
                return UNKNOWN_COMMAND_CODE;
            }
        }
    }
    return NO_ERRORS;
}
#undef DEF_CMD
