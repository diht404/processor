#include "disassembler.h"

size_t printArg(Code *code, FILE *fp, const char *command_name, size_t *ip)
{
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

#define DEF_CMD(name, num, arg)         \
case COMMAND_CODES::CMD_##name:         \
{                                       \
    if (arg)                            \
    {                                   \
        printArg(code, fp, #name, &ip); \
        break;                          \
    }                                   \
    fprintf(fp, #name"\n");             \
    ip++;                               \
    break;                              \
}

size_t disassemle(Code *code, FILE *fp)
{
    assert(code != nullptr);
    assert(fp != nullptr);

    size_t ip = 0;
    while (ip < code->len)
    {
        switch (code->code[ip] & CMD_MASK)
        {
#include "cmd.h"
            default:
            {
                return UNKNOWN_COMMAND_CODE;
            }
        }
    }
    return NO_ERRORS;
}
#undef DEF_CMD


int main(int argc, char *argv[])
{
    size_t error = NO_ERRORS;
    const char *input_filename = "data.code";
    const char *output_filename = "data.disasm";

    if (argc == 3)
    {
        input_filename = argv[1];
        output_filename = argv[2];
    }
    if (argc > 3)
    {
        return -1;
    }
    setbuf(stdout, NULL);

    FILE *fp = fopen(input_filename, "rb");

    Code code = {};
    readCode(fp, &code);

    FILE *fp_out = fopen(output_filename, "w");
    disassemle(&code, fp_out);

    fclose(fp);

    return 0;
}
