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
            case HLT:
            {
                ip = code->len;
                fprintf(fp, "hlt");
                break;
            }
            case PUSH:
            {
                uint8_t cmd = code->code[ip];
                ip++;
                int value = *(int *)(code->code+ip);

                if (cmd & ARG_MASK)
                {
                    if (cmd & REG_MASK)
                    {
                        if (value == 1)
                            fprintf(fp, "push rax\n");
                        else if (value == 2)
                            fprintf(fp, "push rbx\n");
                        else if (value == 3)
                            fprintf(fp, "push rcx\n");
                        else if (value == 4)
                            fprintf(fp, "push rdx\n");
                        else
                            return UNKNOWN_REG;
                    }
                    if (cmd & IMM_MASK)
                    {
                        fprintf(fp, "push %d\n", value);
                    }
                }
                ip += sizeof(int);
                break;
            }
            case POP:
            {
                uint8_t cmd = code->code[ip];
                ip++;
                int value = *(int *)(code->code+ip);

                if (cmd & ARG_MASK)
                {
                    if (cmd & REG_MASK)
                    {
                        if (value == 1)
                            fprintf(fp, "pop rax\n");
                        else if (value == 2)
                            fprintf(fp, "pop rbx\n");
                        else if (value == 3)
                            fprintf(fp, "pop rcx\n");
                        else if (value == 4)
                            fprintf(fp, "pop rdx\n");
                        else
                            return UNKNOWN_REG;
                    }
                    if (cmd & IMM_MASK)
                    {
                        fprintf(fp, "push %d\n", value);
                    }
                }
                ip += sizeof(int);
                break;
            }
            case ADD:
            {
                fprintf(fp, "add\n");
                ip++;
                break;
            }
            case SUB:
            {
                fprintf(fp, "sub\n");
                ip++;
                break;
            }
            case MUL:
            {
                fprintf(fp, "mul\n");
                ip++;
                break;
            }
            case DIV:
            {
                fprintf(fp, "div\n");
                ip++;
                break;
            }
            case OUT:
            {
                fprintf(fp, "out\n");
                ip++;
                break;
            }
            case DUMP:
            {
                fprintf(fp, "dump\n");
                ip++;
                break;
            }
            case IN:
            {
                fprintf(fp, "in\n");
                ip++;
                break;
            }
            default:
            {
                return UNKNOWN_COMMAND;
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
