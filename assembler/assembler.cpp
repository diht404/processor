#include "assembler.h"

size_t compileWithNamesTable(AsmProgram *program,
                             Code *code)
{
    size_t error = NO_ERRORS;

    CHECK_NULLPTR_ERROR(program, PROGRAM_IS_NULLPTR)

    NameTable table = {};
    code->nameTable = table;

    error = compile(program, code);
    if (error)
        return error;
    // add names from name table
    error = compile(program, code);
    return error;
}

#define DEF_CMD(name, num, arg, cpu_code)           \
else if (strcasecmp(cmd, #name) == 0)               \
{                                                   \
    if (!arg)                                       \
    {                                               \
        *code->code = COMMAND_CODES::CMD_##name;    \
        lenOfCode++;                                \
        code->code++;                               \
    }                                               \
    else                                            \
        error = putCommandAndArgsToCodeArray(       \
                        program,                    \
                        line,                       \
                        code,                       \
                        &commandSize,               \
                        &lenOfCode,                 \
                        COMMAND_CODES::CMD_##name); \
}

size_t compile(AsmProgram *program,
               Code *code)
{
    size_t error = NO_ERRORS;

    CHECK_NULLPTR_ERROR(program, PROGRAM_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)

    size_t line = 0;
    char cmd[BUFFER_SIZE] = "";

    code->code =
        (uint8_t *) calloc(2 * program->length * sizeof(int), 1);
    if (code->code == nullptr)
    {
        error |= ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM;
        return error;
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
                error |= ASSEMBLER_COMPILATION_FAILED;
            return error;
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
                error |= ASSEMBLER_COMPILATION_FAILED;
            return error;
        }
        line++;
    }
    code->code -= lenOfCode;

    addInfo(code, lenOfCode);
    uint8_t *newMemory = (uint8_t *) realloc(code->code,
                                             lenOfCode
                                                 * sizeof(newMemory[0]));
    if (newMemory == nullptr)
    {
        if (error)
            error = ASSEMBLER_CANT_SHRINK_TO_FIT;
        return error;
    }
    code->code = newMemory;
    return error;
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

size_t putCommandAndArgsToCodeArray(AsmProgram *program,
                                    size_t line,
                                    Code *code,
                                    int *commandSize,
                                    int *lenOfCode,
                                    int command_code)
{
    size_t error = NO_ERRORS;

    CHECK_NULLPTR_ERROR(program, PROGRAM_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code->code, PROGRAM_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(commandSize, TOKEN_PTR_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(lenOfCode, LEN_OF_CODE_IS_NULLPTR)

    char buffer[BUFFER_SIZE] = "";
    int value = 0;

    skipSpaces(program, line, commandSize);

    error |= detectBrackets(program,
                            code,
                            *commandSize,
                            buffer,
                            line);
    if (error)
        return error;

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
        error |= processArgs(code,
                             command_code,
                             buffer,
                             lenOfCode,
                             value);
    return error;
}

size_t detectBrackets(AsmProgram *program,
                      Code *code,
                      int commandSize,
                      char *buffer,
                      size_t line)
{
    size_t error = NO_ERRORS;

    CHECK_NULLPTR_ERROR(program, PROGRAM_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(buffer, BUFFER_IS_NULLPTR)

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
            return INCORRECT_BRACKETS;
        }
        memcpy(buffer,
               program->lines[line] + bracket + 1,
               (size_t) (commandSize - bracket - 1));
        *((*code).code) |= RAM_MASK;
//        fprintf(stderr, "ram detected %zu %d %d\n", line, *code->code & RAM_MASK, ARG_MASK);
    }
    else
    {
        memcpy(buffer,
               (program->lines[line] + commandSize),
               BUFFER_SIZE);
    }
    return error;
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

size_t processArgs(Code *code,
                   int command_code,
                   char *buffer,
                   int *lenOfCode,
                   int value)
{
    size_t error = NO_ERRORS;

    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code->code, CODE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(buffer, BUFFER_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)

    bool success = false;
    compileRegs(code,
                command_code,
                buffer,
                lenOfCode,
                &success);
    if (success)
        return error;

    if (!sscanf(buffer,
                "%d",
                &value))
    {
        return ASSEMBLER_COMPILATION_FAILED;
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
    return error;
}

size_t compileRegs(Code *code,
                   int command_code,
                   char *buffer,
                   int *lenOfCode,
                   bool *success)
{
    size_t error = NO_ERRORS;

    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code->code, CODE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(buffer, BUFFER_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)

    for (int i = 1; i < sizeof(REGS_NAMES) / sizeof(REGS_NAMES[0]); i++)
    {
        if (strcasecmp(buffer, REGS_NAMES[i]) == 0)
        {
            *code->code |= (command_code) | REG_MASK;
            (*lenOfCode)++;
            code->code++;
            *(int *) code->code = i;
            *lenOfCode += sizeof(int);
            code->code += sizeof(int);
            *success = true;
            return error;
        }
    }
    return NO_ERRORS;
}

void addInfo(Code *code, int lenOfCode)
{
    if (code == nullptr)
    {
        return;
    }

    code->codeHeader.compilation_const = COMPILATION_CONST;
    code->codeHeader.version = VERSION_CONST;
    code->codeHeader.length = (size_t) lenOfCode;
}

size_t readFile(FILE *fp, AsmProgram *program)
{
    CHECK_NULLPTR_ERROR(fp, FILE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(program, PROGRAM_IS_NULLPTR)

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
    CHECK_NULLPTR_ERROR(code, CODE_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(filename, FILENAME_IS_NULLPTR)

    FILE *fp = fopen(filename, "w");

    saveHeader(&code->codeHeader, fp);

    size_t numElements = code->codeHeader.length;
    fwrite(code->code, sizeof(uint8_t), numElements, fp);

    fclose(fp);
    return NO_ERRORS;
}

size_t saveHeader(CodeHeader *header, FILE *fp)
{
    CHECK_NULLPTR_ERROR(header, HEADER_IS_NULLPTR)
    CHECK_NULLPTR_ERROR(fp, FILE_IS_NULLPTR)

    fwrite(header, sizeof(*header), 1, fp);

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