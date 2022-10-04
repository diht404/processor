#include "utils.h"

int openFile(const char *filename, const char *mode, FILE **fp)
{
    assert(filename != nullptr);
    assert(mode != nullptr);
    assert(fp != nullptr);

    *fp = fopen(filename, mode);
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
