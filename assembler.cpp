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

void addInfo(uint8_t **code)
{
    assert(code != nullptr);
    assert(*code != nullptr);

    *(size_t *) *code = COMPILATION_CONST;
    *code = (uint8_t *) ((size_t *) *code + 1);

    *(size_t *) *code = VERSION_CONST;
    *code = (uint8_t *) ((size_t *) *code + 1);

    *code = (uint8_t *) ((size_t *) *code + 1);
}

void skipSpaces(Program *program, size_t line, size_t *commandSize)
{
    while (*(program->lines[line] + *commandSize) == ' '
        or *(program->lines[line] + *commandSize) == '\0'
        or *(program->lines[line] + *commandSize) == '\n')
    {
        (*commandSize)++;
    }
}

void detectBrackets(Program *program,
                    uint8_t *code,
                    size_t commandSize,
                    char *buffer,
                    size_t line,
                    size_t *error)
{
    if (*(program->lines[line] + commandSize) == '[')
    {
        size_t bra = commandSize;
        size_t ket = commandSize;

        bool correct = false;
        while (!(*(program->lines[line] + ket) != ']'
            and *(program->lines[line] + ket) != '\0'
            and *(program->lines[line] + ket) == '\n'))
        {
            if (*(program->lines[line] + ket) == ']')
                correct = true;
            ket++;
        }

        if (!correct)
        {
            *error |= INCORRECT_BRACKETS;
            return;
        }
        memcpy(buffer,
               program->lines[line] + bra + 1,
               ket - bra);
        *code |= RAM_MASK;
        // TODO: add [rax + value] support
    }
    else
    {
        memcpy(buffer,
               (program->lines[line] + commandSize),
               BUFFER_SIZE);
    }
}

void writeCommand(uint8_t **code, int *lenOfCode, int command)
{
    **code = command;
    (*lenOfCode)++;
    (*code)++;
}

void processArgs(uint8_t **code,
                 int command_code,
                 char *buffer,
                 int *lenOfCode,
                 int value,
                 size_t *error)
{
    reg_compile(command_code, "rax", 1)
    else reg_compile(command_code, "rbx", 2)
    else reg_compile(command_code, "rcx", 3)
    else reg_compile(command_code, "rdx", 4)
    else if (!sscanf(buffer,
                     "%d",
                     &value))
    {
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

uint8_t *compile(Program *program,
                 NamesTable *table,
                 size_t *error)
{
    assert(program != nullptr);
    assert(error != nullptr);

    size_t line = 0;
    char cmd[BUFFER_SIZE] = "";

    size_t constLen = sizeof(COMPILATION_CONST);

    uint8_t *code = (uint8_t *) calloc(constLen * 3 +
        2 * program->length * sizeof(int), 1);
    if (code == nullptr)
    {
        *error |= ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM;
        return nullptr;
    }

    addInfo(&code);

    int lenOfCode = 0;

    while (line < program->length)
    {
        size_t commandSize = 0;
        skipSpaces(program, line, &commandSize);
        if (!sscanf(program->lines[line],
                    "%s",
                    cmd))
        {
            *error |= ASSEMBLER_COMPILATION_FAILED;
            return nullptr;
        }
        // label definition
        if (cmd[0] == ':')
        {
            fillNameTable(table, cmd, lenOfCode);
        }
        else if (stricmp(cmd, "push") == 0)
        {
            char buffer[BUFFER_SIZE] = "";
            int value = 0;
            commandSize += 4;

            skipSpaces(program, line, &commandSize);

            detectBrackets(program,
                           code,
                           commandSize,
                           buffer,
                           line,
                           error);
            if (*error)
                return nullptr;

            processArgs(&code,
                        COMMAND_CODES::PUSH,
                        buffer,
                        &lenOfCode,
                        value,
                        error);

        }
        else if (stricmp(cmd, "add") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::ADD);
        else if (stricmp(cmd, "sub") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::SUB);
        else if (stricmp(cmd, "mul") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::MUL);
        else if (stricmp(cmd, "div") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::DIV);
        else if (stricmp(cmd, "OUT") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::OUT);
        else if (stricmp(cmd, "hlt") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::HLT);
        else if (stricmp(cmd, "dump") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::DUMP);
        else if (stricmp(cmd, "in") == 0)
            writeCommand(&code, &lenOfCode, COMMAND_CODES::IN);
        else if (stricmp(cmd, "pop") == 0)
        {
            commandSize += 3;
            char buffer[BUFFER_SIZE] = "";
            int value = 0;

            skipSpaces(program, line, &commandSize);

            detectBrackets(program,
                           code,
                           commandSize,
                           buffer,
                           line,
                           error);
            if (*error)
                return nullptr;

            processArgs(&code,
                        COMMAND_CODES::POP,
                        buffer,
                        &lenOfCode,
                        value,
                        error);
        }
        else if (stricmp(cmd, "jmp") == 0)
        {
            commandSize += 4;
            int value = 0;
            char buffer[BUFFER_SIZE] = "";
            skipSpaces(program, line, &commandSize);

            detectBrackets(program,
                           code,
                           commandSize,
                           buffer,
                           line,
                           error);
            if (*error)
                return nullptr;
            if (buffer[0] == ':')
            {
                value = getIpFromTable(table, buffer);
                *code |= COMMAND_CODES::JMP | IMM_MASK;

                lenOfCode++;
                code++;
                *(int *) code = value;

                lenOfCode += sizeof(int);
                code += sizeof(int);
            }
            else
                processArgs(&code,
                            COMMAND_CODES::JMP,
                            buffer,
                            &lenOfCode,
                            value,
                            error);
        }
        else
        {
            *error |= ASSEMBLER_COMPILATION_FAILED;
            return nullptr;
        }
        line++;
    }
    code -= lenOfCode;

    *(((size_t *) code) - 1) = lenOfCode;
    code = (uint8_t *) (((size_t *) code) - 3);

    uint8_t *newMemory = (uint8_t *) realloc(code,
                                             constLen * 3 + lenOfCode
                                                 * sizeof(newMemory[0]));
    if (newMemory == nullptr)
    {
        *error = ASSEMBLER_CANT_SHRINK_TO_FIT;
        return code;
    }
    return code;
}

void fillNameTable(NamesTable *table,
                   char name[BUFFER_SIZE],
                   int ip)
{
    bool exist = false;
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (stricmp(table->names_table[i], name) == 0)
        {
            exist = true;
            break;
        }
    }
    if (!exist)
    {
        for (int i = 0; i < BUFFER_SIZE; i++)
        {
            if (stricmp(table->names_table[i], "") == 0)
            {
                memcpy(table->names_table[i], name, BUFFER_SIZE);
                table->positions[i] = ip;
                return;
            }
        }
    }
}

int getIpFromTable(NamesTable *table,
                   char name[BUFFER_SIZE])
{
    for (int i = 0; i < BUFFER_SIZE; i++)
    {
        if (stricmp(table->names_table[i], name) == 0)
        {
            return table->positions[i];
        }
    }
    return -1;
}

uint8_t *compileWithNamesTable(Program *program,
                               size_t *error)
{
    NamesTable table = {};
    // [id][name]

    uint8_t *code = nullptr;

    code = compile(program, &table, error);
    // add names from names table
    code = compile(program, &table, error);

    return code;
}

size_t saveFile(uint8_t *code, const char *filename)
{
    assert(code != nullptr);
    assert(filename != nullptr);

    FILE *fp = fopen(filename, "w");

    size_t numElements =
        3 * sizeof(size_t) + *(((size_t *) (code)) + 2);

    fwrite(code, sizeof(uint8_t), numElements, fp);

    fclose(fp);
    return NO_ERRORS;
}

size_t saveFileTxt(uint8_t *code, const char *filename)
{
    assert(code != nullptr);
    assert(filename != nullptr);

    FILE *fp = fopen(filename, "w");
    if (fp == nullptr)
        return 1;

    size_t compilationConst = *((size_t *) code);
    size_t version = *(((size_t *) code) + 1);
    size_t length = *(((size_t *) code) + 2);
    code = (uint8_t *) (((size_t *) code) + 3);

    fprintf(fp,
            "%zu %zu %zu\n",
            compilationConst,
            version,
            length);

    size_t ip = 0;
    while (ip < length)
    {
        if ((code[ip] & CMD_MASK) == HLT)
        {
            fprintf(fp, "%hhu", code[ip]);
            break;
        }
        else if ((code[ip] & CMD_MASK) == PUSH
            || (code[ip] & CMD_MASK) == POP
            || (code[ip] & CMD_MASK) == JMP)
        {
            fprintf(fp,
                    "%hhu %d\n",
                    code[ip],
                    *(int *) (code + ip + 1));
            ip += 1 + sizeof(int);
        }
        else
        {
            fprintf(fp, "%hhu\n", code[ip]);
            ip += sizeof(char);
        }
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

    uint8_t *code = compileWithNamesTable(&text, &error);

    if (error)
        printf("compile error: %zu\n", error);

    saveFile(code, "data.code");
    error = saveFileTxt(code, "data.txt");
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