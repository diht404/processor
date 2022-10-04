#ifndef STACK__ASSEMBLER_H
#define STACK__ASSEMBLER_H

#include "stack/stack.h"
#include <sys/stat.h>

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

enum ASSEMBLER_ERRORS
{
    NO_ERRORS = 0,
    CANT_GET_FILE_INFO = 1,
    CANT_ALLOCATE_MEMORY_FOR_FILE = 2,
    CANT_OPEN_FILE = 3,
    CANT_ALLOCATE_MEMORY_FOR_PROGRAMM = 4,
    CANT_ALLOCATE_MEMORY_FOR_STRINGS = 5,
    CANT_ALLOCATE_MEMORY_FOR_STRINGS_LENGTH = 6,
    COMPILATION_FAILED = 7,
    READ_FAILED = 8,
};

/**
 * @brief Struct for storing a string and its' length
 */
struct Line
{
    char *str = nullptr;
    size_t length = 0;
};

/**
 * @brief Struct for storing array of strings, its' length and pointer
 * to memory block with not sorted text(array of string)
 */
struct Text
{
    Line *lines = nullptr;
    size_t length = 0;
    char *txt = nullptr;
    size_t *lensOfStrings = nullptr;
};
/**
 * @brief opens file
 *
 * @param filename name of the file to open
 * @param mode mode to open the file
 * @param fp file
 * @return error code
 */
int openFile(const char *filename, const char *mode, FILE **fp);

/**
 * @brief return length of file in bytes
 *
 * @param fp - opened file
 * @param lenOfFile - length of file
 * @return error code
 */
int getLenOfFile(FILE *fp, size_t *lenOfFile);

/**
 * reads file to buffer
 * @param fp - opened file
 * @param lenOfFile - variable for storing length of file
 * @param txt - array storing file
 * @return error code
 */
int readFileToBuf(FILE *fp, size_t *lenOfFile, char **txt);

/**
 * @brief counts lines in buffer
 * @param txt - buffer with text
 * @param lenOfFile - len of buffer
 * @return number of lines in buffer
 */
size_t countLines(const char *txt, size_t lenOfFile);

/**
 * reads file to struct Text
 * @param fp - opened file
 * @param text - struct Text with file
 * @return error code
 */
int readFile(FILE *fp, Text *text);

#endif //STACK__ASSEMBLER_H
