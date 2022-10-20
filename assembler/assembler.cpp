#include <cctype>
#include "assembler.h"

uint8_t *compileWithNamesTable(Program *program,
                               size_t *error)
{
    if (program == nullptr)
    {
        if (error)
            *error |= PROGRAM_IS_NULLPTR;
        return nullptr;
    }

    NamesTable table = {};

    uint8_t *code = nullptr;

    code = compile(program, &table, error);
    free(code);
    // add names from names table
    code = compile(program, &table, error);

    return code;
}

#define DEF_CMD(name, num, arg, cpu_code)  \
else if (strcasecmp(cmd, #name) == 0)      \
{                                          \
    if (!arg)                              \
    {                                      \
        *code = COMMAND_CODES::CMD_##name; \
        lenOfCode++;                       \
        code++;                            \
    }                                      \
    else putArgs(program,                  \
            line,                          \
            &code,                         \
            &commandSize,                  \
            &lenOfCode,                    \
            COMMAND_CODES::CMD_##name,     \
            table,                         \
            error);                        \
}

uint8_t *compile(Program *program,
                 NamesTable *table,
                 size_t *error)
{
    if (program == nullptr)
    {
        if (error)
            *error |= PROGRAM_IS_NULLPTR;
        return nullptr;
    }

    if (table == nullptr)
    {
        if (error)
            *error |= NAME_TABLE_IS_NULLPTR;
        return nullptr;
    }

    size_t line = 0;
    char cmd[BUFFER_SIZE] = "";

    size_t constLen = sizeof(COMPILATION_CONST);

    uint8_t *code = (uint8_t *) calloc(constLen * 3 +
        2 * program->length * sizeof(int), 1);
    if (code == nullptr)
    {
        if (error)
            *error |= ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM;
        return nullptr;
    }

    int lenOfCode = 0;
    code = (uint8_t *)((size_t *)code + 3);

    while (line < program->length)
    {
        int commandSize = 0;
        skipSpaces(program, line, &commandSize);
        if (!sscanf(program->lines[line],
                    "%s%n",
                    cmd,
                    &commandSize))
        {
            if (error)
                *error |= ASSEMBLER_COMPILATION_FAILED;
            return nullptr;
        }
        // label definition
        if (cmd[0] == ':')
        {
            fillNameTable(table, cmd, lenOfCode);
        }
#include "../common/cmd.h"
        else
        {
            if (error)
                *error |= ASSEMBLER_COMPILATION_FAILED;
            return nullptr;
        }
        line++;
    }
    code -= lenOfCode;

//    *(((size_t *) code) - 1) = lenOfCode;
    code = (uint8_t *) (((size_t *) code) - 3);
    addInfo(&code, lenOfCode);

    uint8_t *newMemory = (uint8_t *) realloc(code,
                                             constLen * 3 + lenOfCode
                                                 * sizeof(newMemory[0]));
    if (newMemory == nullptr)
    {
        if (error)
            *error = ASSEMBLER_CANT_SHRINK_TO_FIT;
        return code;
    }
    return code;
}
#undef DEF_CMD

void fillNameTable(NamesTable *table,
                   char name[BUFFER_SIZE],
                   int ip)
{
    bool exist = false;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (strcasecmp(table->names_table[i], name) == 0)
        {
            exist = true;
            break;
        }
    }
    if (!exist)
    {
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (strcasecmp(table->names_table[i], "") == 0)
            {
                memcpy(table->names_table[i], name, BUFFER_SIZE);
                table->positions[i] = ip;
                return;
            }
        }
    }
}

void putArgs(Program *program,
             size_t line,
             uint8_t **code,
             int *commandSize,
             int *lenOfCode,
             int command_code,
             NamesTable *table,
             size_t *error)
{
    if (program == nullptr)
    {
        if (error)
            *error |= PROGRAM_IS_NULLPTR;
        return;
    }

    if (code == nullptr)
    {
        if (error)
            *error |= CODE_IS_NULLPTR;
        return;
    }

    if (*code == nullptr)
    {
        if (error)
            *error |= CODE_IS_NULLPTR;
        return;
    }

    if (commandSize == nullptr)
    {
        if (error)
            *error |= COMMAND_SIZE_IS_NULLPTR;
        return;
    }

    if (lenOfCode == nullptr)
    {
        if (error)
            *error |= LEN_OF_CODE_IS_NULLPTR;
        return;
    }

    if (table == nullptr)
    {
        if (error)
            *error |= NAME_TABLE_IS_NULLPTR;
        return;
    }

    char buffer[BUFFER_SIZE] = "";
    int value = 0;

    skipSpaces(program, line, commandSize);

    detectBrackets(program,
                   *code,
                   *commandSize,
                   buffer,
                   line,
                   error);
    if (*error)
        return;

    if (buffer[0] == ':')
    {
        value = getIpFromTable(table, buffer);
        **code |= command_code | IMM_MASK;

        (*lenOfCode)++;
        (*code)++;
        **(int **) code = value;

        *lenOfCode += sizeof(int);
        *code += sizeof(int);
    }
    else
        processArgs(code,
                    command_code,
                    buffer,
                    lenOfCode,
                    value,
                    error);
}

void detectBrackets(Program *program,
                    uint8_t *code,
                    int commandSize,
                    char *buffer,
                    size_t line,
                    size_t *error)
{
    if (program == nullptr)
    {
        if (error)
            *error |= PROGRAM_IS_NULLPTR;
        return;
    }

    if (code == nullptr)
    {
        if (error)
            *error |= CODE_IS_NULLPTR;
        return;
    }

    if (buffer == nullptr)
    {
        if (error)
            *error |= BUFFER_IS_NULLPTR;
        return;
    }

    if (*(program->lines[line] + commandSize) == '[')
    {
        int bracket = commandSize;

        bool correct = false;
        while (commandSize < BUFFER_SIZE)
        {
            if (*(program->lines[line] + commandSize) == ']')
            {
                correct = true;
                break;
            }
            else
                commandSize++;
        }
        if (!correct)
        {
            *error |= INCORRECT_BRACKETS;
            return;
        }
        memcpy(buffer,
               program->lines[line] + bracket + 1,
               (size_t)(commandSize - bracket - 1));
        *code |= RAM_MASK;
    }
    else
    {
        memcpy(buffer,
               (program->lines[line] + commandSize),
               BUFFER_SIZE);
    }
}

int getIpFromTable(NamesTable *table,
                   char name[BUFFER_SIZE])
{
    if (table == nullptr)
    {
        return -1;
    }
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (strcasecmp(table->names_table[i], name) == 0)
        {
            return table->positions[i];
        }
    }
    return -1;
}


void processArgs(uint8_t **code,
                 int command_code,
                 char *buffer,
                 int *lenOfCode,
                 int value,
                 size_t *error)
{
    if (code == nullptr)
    {
        if (error)
            *error |= CODE_IS_NULLPTR;
        return;
    }

    if (*code == nullptr)
    {
        if (error)
            *error |= CODE_IS_NULLPTR;
        return;
    }

    if (buffer == nullptr)
    {
        if (error)
            *error |= BUFFER_IS_NULLPTR;
        return;
    }

    reg_compile(command_code, "rax", 1)
    else reg_compile(command_code, "rbx", 2)
    else reg_compile(command_code, "rcx", 3)
    else reg_compile(command_code, "rdx", 4)
    else if (!sscanf(buffer,
                     "%d",
                     &value))
    {
        if (error)
            *error |= ASSEMBLER_COMPILATION_FAILED;
        return;
    }
    else
    {
        **code |= command_code | IMM_MASK;
        (*lenOfCode)++;
        (*code)++;
        **(int **) code = value;

        *lenOfCode += sizeof(int);
        *code += sizeof(int);
    }
}

void skipSpaces(Program *program, size_t line, int *commandSize)
{
    if (program == nullptr)
    {
        return;
    }

    if (commandSize == nullptr)
    {
        return;
    }

    while (*(program->lines[line] + *commandSize) == ' '
        or *(program->lines[line] + *commandSize) == '\0'
        or *(program->lines[line] + *commandSize) == '\n')
    {
        (*commandSize)++;
    }
}

void addInfo(uint8_t **code, int lenOfCode)
{
    if (code == nullptr)
    {
        return;
    }

    if (*code == nullptr)
    {
        return;
    }

    *(size_t *) *code = COMPILATION_CONST;
    *code = (uint8_t *) ((size_t *) *code + 1);

    *(size_t *) *code = VERSION_CONST;
    *code = (uint8_t *) ((size_t *) *code + 1);

    *(size_t *) *code = lenOfCode;
    *code = (uint8_t *) ((size_t *) *code - 2);
}

size_t readFile(FILE *fp, Program *program)
{
    if (fp == nullptr)
    {
        return FILENAME_IS_NULLPTR;
    }

    if (program == nullptr)
    {
        return PROGRAM_IS_NULLPTR;
    }

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

size_t saveFile(uint8_t *code, const char *filename)
{
    if (code == nullptr)
    {
        return CODE_IS_NULLPTR;
    }
    if (filename == nullptr)
    {
        return FILENAME_IS_NULLPTR;
    }

    FILE *fp = fopen(filename, "w");

    size_t numElements =
        3 * sizeof(size_t) + *(((size_t *) (code)) + 2);

    fwrite(code, sizeof(uint8_t), numElements, fp);

    fclose(fp);
    return NO_ERRORS;
}

void processAsmError(size_t error)
{
    FILE *fp = stderr;
    if (!error)
    {
        return;
    }

    if (error & ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM)
        fprintf(fp, "Can't allocate memory for program.\n");
    if (error & ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_STRINGS)
        fprintf(fp, "Can't allocate memory for strings.\n");
    if (error & ASSEMBLER_COMPILATION_FAILED)
        fprintf(fp, "Compilation failed.\n");
    if (error & ASSEMBLER_CANT_SHRINK_TO_FIT)
        fprintf(fp, "Can't shrink array with code to fit.\n");
    if (error & INCORRECT_BRACKETS)
        fprintf(fp, "Got incorrect brackets.\n");
    if (error & CODE_IS_NULLPTR)
        fprintf(fp, "Code is nullptr.\n");
    if (error & BUFFER_IS_NULLPTR)
        fprintf(fp, "Buffer is nullptr.\n");
    if (error & PROGRAM_IS_NULLPTR)
        fprintf(fp, "Program is nullptr.\n");
    if (error & COMMAND_SIZE_IS_NULLPTR)
        fprintf(fp, "Command size is nullptr.\n");
    if (error & LEN_OF_CODE_IS_NULLPTR)
        fprintf(fp, "Length of code is nullptr.\n");
    if (error & NAME_TABLE_IS_NULLPTR)
        fprintf(fp, "Names table is nullptr.\n");
    if (error & FILENAME_IS_NULLPTR)
        fprintf(fp, "Filename is nullptr.\n");
    if (error & FILE_IS_NULLPTR)
        fprintf(fp, "File is nullptr.\n");
}