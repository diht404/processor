#include "assembler.h"
#include "disassembler.h"

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

    int *code = (int *) calloc(constLen * 3 +
        2 * program->length * sizeof(code[0]), 1);
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

    FILE *fp = fopen(filename, "w");

    size_t numElements =
        3 * sizeof(size_t) / sizeof(int) + *((size_t *) (code) + 2);

    fwrite(code, sizeof(int), numElements, fp);

    fclose(fp);
    return NO_ERRORS;
}

size_t print(Code *code, FILE *fp)
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

size_t saveFileTxt(int *code, const char *filename)
{
    assert(code != nullptr);
    assert(filename != nullptr);

    FILE *fp = fopen(filename, "w");
    if (fp == nullptr)
        return 1;

    size_t compilationConst = *((size_t *) code);
    size_t version = *((size_t *) code + 1);
    size_t length = *((size_t *) code + 2);
    code = (int *)((size_t *) code + 3);

    fprintf(fp,
            "%zu %zu %zu\n",
            compilationConst,
            version,
            length);

    size_t ip = 0;
    while (ip < length)
    {
        if (code[ip] == HLT)
        {
            fprintf(fp, "%d", code[ip]);
            break;
        }
        if (code[ip] == PUSH)
        {
            fprintf(fp, "%d %d\n", code[ip], code[ip+1]);
            ip+=2;
            continue;
        }
        fprintf(fp, "%d\n", code[ip]);
        ip++;
    }

    fclose(fp);
    return NO_ERRORS;
}

int main()
{
    FILE *fp = fopen("data.asm", "r");

    Program text = {};
    readFile(fp, &text);

    size_t error = 0;

    int *code = compile(&text, &error);
    if (error)
        printf("compile error: %zu\n", error);
    saveFile(code, "data.code");
    error = saveFileTxt(code, "data.txt");
    free(code);
    fclose(fp);
    return 0;
}
