#include "disassembler.h"

size_t disassemle(Code *code, FILE *fp)
{
    assert(code != nullptr);
    assert(fp != nullptr);

    size_t ip = 0;
    while (ip < code->len)
    {
        switch (code->code[ip] & CMD_MASK)
        {
            case COMMAND_CODES::HLT:
            {
                ip = code->len;
                fprintf(fp, "hlt");
                break;
            }
            case COMMAND_CODES::ADD:
            {
                fprintf(fp, "add\n");
                ip++;
                break;
            }
            case COMMAND_CODES::SUB:
            {
                fprintf(fp, "sub\n");
                ip++;
                break;
            }
            case COMMAND_CODES::MUL:
            {
                fprintf(fp, "mul\n");
                ip++;
                break;
            }
            case COMMAND_CODES::DIV:
            {
                fprintf(fp, "div\n");
                ip++;
                break;
            }
            case COMMAND_CODES::OUT:
            {
                fprintf(fp, "out\n");
                ip++;
                break;
            }
            case COMMAND_CODES::DUMP:
            {
                fprintf(fp, "dump\n");
                ip++;
                break;
            }
            case COMMAND_CODES::IN:
            {
                fprintf(fp, "in\n");
                ip++;
                break;
            }
            case COMMAND_CODES::PUSH:
            {
                uint8_t cmd = code->code[ip];
                ip++;
                int value = *(int *) (code->code + ip);

                if (cmd & ARG_MASK)
                {
                    if (cmd & REG_MASK)
                    {
                        if (value > 0 and value < 5)
                            if (cmd & RAM_MASK)
                                fprintf(fp,
                                        "push [%s]\n",
                                        REGS_NAMES[value]);
                            else
                                fprintf(fp,
                                        "push %s\n",
                                        REGS_NAMES[value]);
                        else
                            return UNKNOWN_REG;
                    }
                    if (cmd & IMM_MASK)
                    {
                        if (cmd & RAM_MASK)
                            fprintf(fp, "push [%d]\n", value);
                        else
                            fprintf(fp, "push %d\n", value);
                    }
                }
                ip += sizeof(int);
                break;
            }
            case COMMAND_CODES::POP:
            {
                uint8_t cmd = code->code[ip];
                ip++;
                int value = *(int *) (code->code + ip);

                if (cmd & ARG_MASK)
                {
                    if (cmd & REG_MASK)
                    {
                        if (value > 0 and value < 5)
                            fprintf(fp, "pop %s\n", REGS_NAMES[value]);
                        else
                            return UNKNOWN_REG;
                    }
                    if (cmd & IMM_MASK)
                    {
                        fprintf(fp, "pop %d\n", value);
                    }
                }
                ip += sizeof(int);
                break;
            }
            default:
            {
                return UNKNOWN_COMMAND_CODE;
            }
        }
    }
    return NO_ERRORS;
}

int main()
{
    setbuf(stdout, NULL);

    FILE *fp = fopen("data.code", "rb");

    Code code = {};
    readCode(fp, &code);

    FILE *fp_out = fopen("data.disasm", "w");
    disassemle(&code, fp_out);

    fclose(fp);

    return 0;
}
