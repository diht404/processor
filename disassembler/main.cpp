#include "disassembler.h"

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
    error = disassemble(&code, fp_out);
    if (error)
        processDisasmError(error);
    fclose(fp);

    return 0;
}
