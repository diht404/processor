#ifndef STACK__ASSEMBLER_H
#define STACK__ASSEMBLER_H

#include "utils.h"

enum ASSEMBLER_ERRORS
{
    ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM = 1 << 4,
    ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_STRINGS = 1 << 5,
    ASSEMBLER_COMPILATION_FAILED               = 1 << 6,
    ASSEMBLER_CANT_SHRINK_TO_FIT               = 1 << 7,
    INCORRECT_BRACKETS                         = 1 << 8,
};

/**
 * @brief Struct for storing array of strings, its' length and pointer
 * to memory block with not sorted text(array of string)
 */
struct Program
{
    char **lines  = nullptr;
    size_t length = 0;
};

#define reg_compile(cmd_arg, reg_name, number)     \
    if (stricmp(buffer, (reg_name)) == 0)          \
    {                                              \
        *code = (cmd_arg) | REG_MASK;              \
        lenOfCode++;                               \
        code++;                                    \
        *(int *) code = (number);                  \
        lenOfCode += sizeof(int);                  \
        code += sizeof(int);                       \
    }

/**
 * @brief reads file to struct Program
 *
 * @param fp - opened file
 * @param program - struct Program with file
 * @return error code
 */
size_t readFile(FILE *fp, Program *program);

/**
 * @brief Adds info about code: the compilation const, version, and code length
 *
 * @param code array with code
 */
void addInfo(uint8_t **code);

void skipSpaces(Program *program, size_t line, int *commandSize);

/**
 * @brief Compiles code
 *
 * @param program array with program to compile
 * @param error error code
 * @return compiled code
 */
uint8_t *compile(Program *program, size_t *error);

/**
 * @brief Saves array with compiled code to file
 *
 * @param code code to write to file
 * @param filename name of file to write
 * @return error code
 */
size_t saveFile(uint8_t *code, const char *filename);

/**
 * @brief Saves array with compiled code to txt file
 *
 * @param code code to write to file
 * @param filename name of file to write
 * @return error code
 */
size_t saveFileTxt(uint8_t *code, const char *filename);

#endif //STACK__ASSEMBLER_H
