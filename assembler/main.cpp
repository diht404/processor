#include "assembler.h"

int main(int argc, char *argv[])
{
    size_t error = NO_ERRORS;
    const char *input_filename = "quadratic.asm";
    const char *output_filename = "data.code";

    if (argc == 3)
    {
        input_filename = argv[1];
        output_filename = argv[2];
    }

    FILE *fp = fopen(input_filename, "r");

    Program text = {};
    error = readFile(fp, &text);
    handleAsmError(error);

    uint8_t *code = compileWithNamesTable(&text, &error);
    handleAsmError(error);

    error = saveFile(code, output_filename);
    handleAsmError(error);

    free(code);
    fclose(fp);
    return 0;
}

//push value   value -> stack
//
//push rax     rax   -> stack
//pop  rax     stack -> rax
//
//push []      ram   -> stack
//pop  []      stack -> ram