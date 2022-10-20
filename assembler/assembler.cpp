#include "assembler.h"

void compileWithNamesTable(AsmProgram *program,
                           Code *code,
                           size_t *error)
{
    if (program == nullptr)
    {
        if (error)
            *error |= PROGRAM_IS_NULLPTR;
        return;
    }

    NameTable table = {};
    code->nameTable = table;

    compile(program, code, error);
    // add names from names table
    compile(program, code, error);
}

#define DEF_CMD(name, num, arg, cpu_code)        \
else if (strcasecmp(cmd, #name) == 0)            \
{                                                \
    if (!arg)                                    \
    {                                            \
        *code->code = COMMAND_CODES::CMD_##name; \
        lenOfCode++;                             \
        code->code++;                            \
    }                                            \
    else putArgs(program,                        \
            line,                                \
            code,                                \
            &commandSize,                        \
            &lenOfCode,                          \
            COMMAND_CODES::CMD_##name,           \
            error);                              \
}

void compile(AsmProgram *program,
             Code *code,
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

    size_t line = 0;
    char cmd[BUFFER_SIZE] = "";

    code->code = (uint8_t *) calloc(2 * program->length * sizeof(int), 1);
    if (code->code == nullptr)
    {
        if (error)
            *error |= ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM;
        return;
    }

    int lenOfCode = 0;

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
            return;
        }
        // label definition
        if (cmd[0] == ':')
        {
            fillNameTable(&code->nameTable, cmd, lenOfCode);
        }
#include "../common/cmd.h"
        else
        {
            if (error)
                *error |= ASSEMBLER_COMPILATION_FAILED;
            return;
        }
        line++;
    }
    code->code -= lenOfCode;

    addInfo(code, lenOfCode);
    uint8_t *newMemory = (uint8_t *) realloc(code->code,
                                             lenOfCode * sizeof(newMemory[0]));
    if (newMemory == nullptr)
    {
        if (error)
            *error = ASSEMBLER_CANT_SHRINK_TO_FIT;
    }
    code->code = newMemory;
}
#undef DEF_CMD

void skipSpaces(AsmProgram *program, size_t line, int *tokenPtr)
{
    if (program == nullptr)
    {
        return;
    }

    if (tokenPtr == nullptr)
    {
        return;
    }

    while (*(program->lines[line] + *tokenPtr) == ' '
        or *(program->lines[line] + *tokenPtr) == '\0'
        or *(program->lines[line] + *tokenPtr) == '\n')
    {
        (*tokenPtr)++;
    }
}

void fillNameTable(NameTable *nameTable,
                   char name[BUFFER_SIZE],
                   int ip)
{
    bool exist = false;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (strcasecmp(nameTable->name_table[i], name) == 0)
        {
            exist = true;
            break;
        }
    }
    if (!exist)
    {
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (strcasecmp(nameTable->name_table[i], "") == 0)
            {
                memcpy(nameTable->name_table[i], name, BUFFER_SIZE);
                nameTable->positions[i] = ip;
                return;
            }
        }
    }
}

void putArgs(AsmProgram *program,
             size_t line,
             Code *code,
             int *commandSize,
             int *lenOfCode,
             int command_code,
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

    if (code->code == nullptr)
    {
        if (error)
            *error |= CODE_IS_NULLPTR;
        return;
    }

    if (commandSize == nullptr)
    {
        if (error)
            *error |= TOKEN_PTR_IS_NULLPTR;
        return;
    }

    if (lenOfCode == nullptr)
    {
        if (error)
            *error |= LEN_OF_CODE_IS_NULLPTR;
        return;
    }

    char buffer[BUFFER_SIZE] = "";
    int value = 0;

    skipSpaces(program, line, commandSize);

    detectBrackets(program,
                   code,
                   *commandSize,
                   buffer,
                   line,
                   error);
    if (*error)
        return;

    if (buffer[0] == ':')
    {
        value = getIpFromTable(&code->nameTable, buffer);
        *code->code |= command_code | IMM_MASK;

        (*lenOfCode)++;
        code->code++;
        *(int *) code->code = value;

        *lenOfCode += sizeof(int);
        code->code += sizeof(int);
    }
    else
        processArgs(code,
                    command_code,
                    buffer,
                    lenOfCode,
                    value,
                    error);
}

void detectBrackets(AsmProgram *program,
                    Code *code,
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
        *code->code |= RAM_MASK;
    }
    else
    {
        memcpy(buffer,
               (program->lines[line] + commandSize),
               BUFFER_SIZE);
    }
}

int getIpFromTable(NameTable *nameTable,
                   char name[BUFFER_SIZE])
{
    if (nameTable == nullptr)
    {
        return -1;
    }
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (strcasecmp(nameTable->name_table[i], name) == 0)
        {
            return nameTable->positions[i];
        }
    }
    return -1;
}

void processArgs(Code *code,
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

    if (code->code == nullptr)
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
        *code->code |= command_code | IMM_MASK;
        (*lenOfCode)++;
        code->code++;
        *(int *) code->code = value;

        *lenOfCode += sizeof(int);
        code->code += sizeof(int);
    }
}

void addInfo(Code *code, int lenOfCode)
{
    if (code == nullptr)
    {
        return;
    }

    code->codeHeader.compilation_const = COMPILATION_CONST;
    code->codeHeader.version = VERSION_CONST;
    code->codeHeader.length = (size_t)lenOfCode;
}

size_t readFile(FILE *fp, AsmProgram *program)
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

size_t saveFile(Code *code, const char *filename)
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

    saveHeader(&code->codeHeader, fp);

    size_t numElements =  code->codeHeader.length;
    fwrite(code->code, sizeof(uint8_t), numElements, fp);

    fclose(fp);
    return NO_ERRORS;
}

size_t saveHeader(CodeHeader *header, FILE *fp)
{
    if (header == nullptr)
        return HEADER_IS_NULLPTR;
    if (fp == nullptr)
        return FILE_IS_NULLPTR;

    fwrite(&header->compilation_const, sizeof(size_t), 1, fp);
    fwrite(&header->version, sizeof(size_t), 1, fp);
    fwrite(&header->length, sizeof(size_t), 1, fp);

    return NO_ERRORS;
}

void handleAsmError(size_t error)
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
        fprintf(fp, "AsmProgram is nullptr.\n");
    if (error & TOKEN_PTR_IS_NULLPTR)
        fprintf(fp, "Command size is nullptr.\n");
    if (error & LEN_OF_CODE_IS_NULLPTR)
        fprintf(fp, "Length of code is nullptr.\n");
    if (error & NAME_TABLE_IS_NULLPTR)
        fprintf(fp, "Names table is nullptr.\n");
    if (error & FILENAME_IS_NULLPTR)
        fprintf(fp, "Filename is nullptr.\n");
    if (error & FILE_IS_NULLPTR)
        fprintf(fp, "File is nullptr.\n");
    if (error & HEADER_IS_NULLPTR)
        fprintf(fp, "Header of file is nullptr.\n");
}