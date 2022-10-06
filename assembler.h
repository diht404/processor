#ifndef STACK__ASSEMBLER_H
#define STACK__ASSEMBLER_H

#include "utils.h"

enum COMMAND_CODES
{
    HLT = 0,
    PUSH = 1,
    ADD = 2,
    SUB = 3,
    MUL = 4,
    DIV = 5,
    OUT = 6,
    DUMP = 7,
    IN = 8,
};

const size_t COMPILATION_CONST = 0xDEADF00D;
const size_t VERSION = 0;

/**
 * @brief Struct for storing a string and its' length
 */
struct Line
{
    char *str = nullptr;
};

/**
 * @brief Struct for storing array of strings, its' length and pointer
 * to memory block with not sorted text(array of string)
 */
struct Text
{
    Line *lines = nullptr;
    size_t length = 0;
};

/**
 * reads file to struct Text
 * @param fp - opened file
 * @param text - struct Text with file
 * @return error code
 */
int readFile(FILE *fp, Text *text);

void addInfo(int **code);

int *compile(Text *text, size_t *error);

size_t saveFile(int *code, const char *filename);

#endif //STACK__ASSEMBLER_H
