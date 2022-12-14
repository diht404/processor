#include "utils.h"

size_t readCode(FILE *fp, Code *code)
{
    assert(fp != nullptr);
    assert(code != nullptr);

    size_t lenOfFile = 0;
    char *buf = nullptr;
    size_t error = readFileToBuf(fp, &lenOfFile, &buf);

    if (error)
        return error;

    error = verifyCode(&buf);
    if (error)
        return error;

    code->len = *((uint32_t *) (buf) + 2);
    code->code = (uint8_t *) ((uint32_t *) (buf) + 3);

    return NO_ERRORS;
}

size_t readFileToBuf(FILE *fp, size_t *lenOfFile, char **txt)
{
    assert(fp != nullptr);
    assert(lenOfFile != nullptr);
    assert(txt != nullptr);

    size_t error = getLenOfFile(fp, lenOfFile);
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

size_t verifyCode(char **buf)
{
    uint32_t compilationConst = *(uint32_t *) (*buf);

    if (compilationConst != COMPILATION_CONST)
    {
        printf("Expected: %u Got: %u\n",
               COMPILATION_CONST,
               compilationConst);
        return NOT_EXECUTABLE_FILE;
    }

    uint32_t version = *((uint32_t *) (*buf) + 1);

    if (version != VERSION_CONST)
        return INCORRECT_VERSION;

    return NO_ERRORS;
}

void freeCode(Code *code)
{
    free(code->code);
}

int get_weekday()
{
    time_t t = time(NULL);
    if (t >= 0)
    {
        return localtime(&t)->tm_wday; // Sunday=0, Monday=1, etc.
    }
    return -1;
}
