#include "disassembler.h"

size_t disassemle(Code *code, FILE *fp)
{
    assert(code != nullptr);
    assert(fp != nullptr);

    size_t ip = 0;
    while (ip < code->len)
    {
        switch (code->code[ip])
        {
            case HLT:
            {
                ip = code->len;
                fprintf(fp, "hlt");
                break;
            }
            case PUSH:
            {
                ip++;
                fprintf(fp, "push %d\n", code->code[ip]);
                ip++;
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
