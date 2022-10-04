#include "assembler.h"

int openFile(const char *filename, const char *mode, FILE **fp)
{
    assert(filename != nullptr);
    assert(mode != nullptr);
    assert(fp != nullptr);

    *fp = fopen(filename, "r");
    if (*fp == nullptr)
    {
        return CANT_OPEN_FILE;
    }
    return NO_ERRORS;
}

int getLenOfFile(FILE *fp, size_t *lenOfFile)
{
    assert(fp != nullptr);
    assert(lenOfFile != nullptr);

    struct stat buff = {};
    if (fstat(fileno(fp), &buff) != 0)
        return CANT_GET_FILE_INFO;

    *lenOfFile = buff.st_size;
    return NO_ERRORS;
}

int readFileToBuf(FILE *fp, size_t *lenOfFile, char **txt)
{
    assert(fp != nullptr);
    assert(lenOfFile != nullptr);
    assert(txt != nullptr);

    int error = getLenOfFile(fp, lenOfFile);
    if (error)
        return error;

    *txt = (char *) calloc(*lenOfFile + 1, sizeof(char));
    if (*txt == nullptr)
    {
        return CANT_ALLOCATE_MEMORY_FOR_FILE;
    }
    fread(*txt, sizeof(char), *lenOfFile, fp);
    return error;
}

size_t countLines(const char *txt, size_t lenOfFile)
{
    assert(txt != nullptr);

    size_t numLines = 1;
    for (size_t i = 0; i < lenOfFile; i++)
    {
        if (txt[i] == '\n')
            numLines++;
    }
    return numLines;
}

int readFile(FILE *fp, Text *text)
{
    assert(fp != nullptr);
    assert(text != nullptr);

    size_t lenOfFile = 0;
    char *txt = nullptr;
    int error = readFileToBuf(fp, &lenOfFile, &txt);

    if (error)
        return error;

    size_t numLines = countLines(txt, lenOfFile);

    Line *lines = (Line *) calloc(numLines + 1, sizeof(lines[0]));
    if (lines == nullptr)
    {
        return CANT_ALLOCATE_MEMORY_FOR_STRINGS;
    }
    size_t *lensOfStrings =
        (size_t *) calloc(numLines + 1, sizeof(lensOfStrings[0]));

    if (lensOfStrings == nullptr)
    {
        return CANT_ALLOCATE_MEMORY_FOR_STRINGS_LENGTH;
    }

    size_t position = 0;
    size_t line_id = 0;

    for (size_t i = 0; i < lenOfFile; i++)
    {
        if (position == 0)
        {
            lines[line_id] = {&txt[i], 0};
        }

        lines[line_id].length++;
        position++;

        if (txt[i] == '\n')
        {
            lines[line_id].length = position;
            lensOfStrings[line_id] = position;
            position = 0;
            txt[i] = '\0';
            line_id++;
        }
    }

    *text = {lines, numLines, txt, lensOfStrings};
    return error;
}

size_t compile(Text *text, Stack *stack)
{
    size_t line = 0;
    char cmd[128] = "";
    int commandSize = 0;
    size_t stackError = 0;
    while (line < text->length)
    {
        if (!sscanf(text->lines[line].str,
                    "%s%n",
                    cmd,
                    &commandSize))
            return COMPILATION_FAILED;
        if (stricmp(cmd, "push") == 0)
        {
            int value = 0;
            if (!sscanf(text->lines[line].str + commandSize,
                        "%d",
                        &value))
                return COMPILATION_FAILED;
            stackError |= stackPush(stack, value);
            if (stackError)
                return stackError;
        }
        else if (stricmp(cmd, "add") == 0)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
                return stackError;

            stackError |= stackPop(stack, &firstValue);
            if (stackError)
                return stackError;

            stackError |= stackPush(stack, firstValue + secondValue);
            if (stackError)
                return stackError;
        }
        else if (stricmp(cmd, "sub") == 0)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
                return stackError;

            stackError |= stackPop(stack, &firstValue);
            if (stackError)
                return stackError;

            stackError |= stackPush(stack, firstValue - secondValue);
            if (stackError)
                return stackError;
        }
        else if (stricmp(cmd, "mul") == 0)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
                return stackError;

            stackError |= stackPop(stack, &firstValue);
            if (stackError)
                return stackError;
            stackError |= stackPush(stack, firstValue * secondValue);
            if (stackError)
                return stackError;
        }
        else if (stricmp(cmd, "div") == 0)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
                return stackError;

            stackError |= stackPop(stack, &firstValue);
            if (stackError)
                return stackError;

            stackError |= stackPush(stack, firstValue / secondValue);
            if (stackError)
                return stackError;
        }
        else if (stricmp(cmd, "out") == 0)
        {
            int value = 0;

            stackError |= stackPop(stack, &value);
            if (stackError)
                return stackError;

            printf("ANSWER = %d\n", value);
        }
        else if (stricmp(cmd, "hlt") == 0)
        {
            return stackError;
        }
        else if (stricmp(cmd, "dump") == 0)
        {
//          TODO: processor dump
            return stackError;
        }
        else if (stricmp(cmd, "in") == 0)
        {
            int value = 0;
            if (!scanf("%d", &value))
                return READ_FAILED;
            stackError |= stackPush(stack, value);
            if (stackError)
                return stackError;
        }
        line++;
    }

}

int main()
{
    FILE *fp = nullptr;
    openFile("1.asm", "r", &fp);

    Text text = {};
    readFile(fp, &text);

    Stack stack = {};
    size_t error = 0;
    stackCtor(&stack, 1, &error)

    compile(&text, &stack);

    fclose(fp);
    return 0;
}
