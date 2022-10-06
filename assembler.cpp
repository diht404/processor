#include "assembler.h"

size_t readFile(FILE *fp, Program *program)
{
    assert(fp != nullptr);
    assert(program != nullptr);

    size_t lenOfFile = 0;
    char *txt = nullptr;
    size_t error = readFileToBuf(fp, &lenOfFile, &txt);
    if (error)
        return error;

    size_t numLines = countLines(txt, lenOfFile);

    char **lines = (char **) calloc(numLines + 1, sizeof(lines[0]));
    if (lines == nullptr)
        return ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_STRINGS;

    size_t position = 0;
    size_t line_id = 0;

    for (size_t i = 0; i < lenOfFile; i++)
    {
        if (position == 0)
        {
            lines[line_id] = {&txt[i]};
        }
        position++;

        if (txt[i] == '\n')
        {
            position = 0;
            txt[i] = '\0';
            line_id++;
        }
    }
    *program = {lines, numLines};

    return error;
}

void addInfo(int **code)
{

    *(size_t *) *code = COMPILATION_CONST;
    *code = (int *) ((size_t *) *code + 1);

    *(size_t *) *code = VERSION;
    *code = (int *) ((size_t *) *code + 1);

    *code = (int *) ((size_t *) *code + 1);
}

int *compile(Program *program, size_t *error)
{
    size_t line = 0;
    char cmd[128] = "";
    int commandSize = 0;

    size_t constLen = sizeof(COMPILATION_CONST);

    int *code =
        (int *) calloc(
            constLen * 3 + 2 * program->length * sizeof(code[0]), 1);
    if (code == nullptr)
    {
        *error |= ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM;
        return nullptr;
    }

    addInfo(&code);

    size_t lenOfCode = 0;

    while (line < program->length)
    {
        if (!sscanf(program->lines[line],
                    "%s%n",
                    cmd,
                    &commandSize))
        {
            *error |= ASSEMBLER_COMPILATION_FAILED;
            return nullptr;
        }

        if (stricmp(cmd, "push") == 0)
        {
            int value = 0;
            if (!sscanf(program->lines[line] + commandSize,
                        "%d",
                        &value))
            {
                *error |= ASSEMBLER_COMPILATION_FAILED;
                return nullptr;
            }

            code[lenOfCode++] = COMMAND_CODES::PUSH;
            code[lenOfCode++] = value;
        }
        else if (stricmp(cmd, "add") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::ADD;
        }
        else if (stricmp(cmd, "sub") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::SUB;
        }
        else if (stricmp(cmd, "mul") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::MUL;
        }
        else if (stricmp(cmd, "div") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::DIV;
        }
        else if (stricmp(cmd, "OUT") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::OUT;
        }
        else if (stricmp(cmd, "hlt") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::HLT;
        }
        else if (stricmp(cmd, "dump") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::DUMP;
        }
        else if (stricmp(cmd, "in") == 0)
        {
            code[lenOfCode++] = COMMAND_CODES::IN;
        }
        else
        {
            *error |= ASSEMBLER_COMPILATION_FAILED;
            return nullptr;
        }
        line++;
    }

    *((size_t *) code - 1) = lenOfCode;
    code = (int *) ((size_t *) code - 3);

    int *newMemory = (int *) realloc(code,
                                     constLen * 3 + lenOfCode
                                         * sizeof(newMemory[0]));
    if (newMemory == nullptr)
    {
        *error = ASSEMBLER_CANT_SHRINK_TO_FIT;
        return code;
    }
    return code;
}

size_t saveFile(int *code, const char *filename)
{
    assert(code != nullptr);
    assert(filename != nullptr);

    FILE *fp = nullptr;
    openFile(filename, "w", &fp);

    size_t numElements =
        3 * sizeof(size_t) / sizeof(int) + *((size_t *) (code) + 2);

    fwrite(code, sizeof(int), numElements, fp);

    fclose(fp);
    return NO_ERRORS;
}

int main()
{
    FILE *fp = nullptr;
    openFile("data.asm", "r", &fp);

    Program text = {};
    readFile(fp, &text);

    size_t error = 0;

    int *code = compile(&text, &error);
    if (error)
        printf("compile error: %zu\n", error);
    saveFile(code, "data.code");

    free(code);
    fclose(fp);
    return 0;
}
