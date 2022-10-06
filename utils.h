#ifndef STACK__UTILS_H
#define STACK__UTILS_H

#include "stack/stack.h"
#include <sys/stat.h>

enum COMMAND_CODES
{
    HLT  = 0,
    PUSH = 1,
    ADD  = 2,
    SUB  = 3,
    MUL  = 4,
    DIV  = 5,
    OUT  = 6,
    DUMP = 7,
    IN   = 8,
};

enum UTILS_ERRORS
{
    NO_ERRORS                     = 0,
    CANT_OPEN_FILE                = 1 << 1,
    CANT_GET_FILE_INFO            = 1 << 2,
    CANT_ALLOCATE_MEMORY_FOR_FILE = 1 << 3,
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

#endif //STACK__UTILS_H
