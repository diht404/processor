#ifndef STACK__UTILS_H
#define STACK__UTILS_H

#include "../stack/stack.h"
#include <sys/stat.h>
#include <unistd.h>
#include "time.h"

#define DEF_CMD(name, num, arg, cpu_code) \
CMD_##name = num,

enum COMMAND_CODES
{
#include "cmd.h"
};

#undef DEF_CMD

#include "consts.h"

enum UTILS_ERRORS
{
    NO_ERRORS                     = 0,
    CANT_GET_FILE_INFO            = 1 << 1,
    CANT_ALLOCATE_MEMORY_FOR_FILE = 1 << 2,
    NOT_EXECUTABLE_FILE           = 1 << 3,
    INCORRECT_VERSION             = 1 << 4,
};

struct NameTable
{
    char name_table[BUFFER_SIZE][BUFFER_SIZE] = {};
    int positions[BUFFER_SIZE] = {};
};

struct CodeHeader
{
    size_t compilation_const = 0;
    size_t version = 0;
    size_t length = 0;
};

struct Code
{
    uint8_t *code = nullptr;
    size_t len = 0;
    NameTable nameTable = {};
    CodeHeader codeHeader = {};
};

/**
 * @brief Reads code from binary file
 *
 * @param fp file to read from
 * @param code struct to read code
 * @return
 */
size_t readCode(FILE *fp, Code *code);

/**
 * reads file to buffer
 * @param fp - opened file
 * @param lenOfFile - variable for storing length of file
 * @param txt - array storing file
 * @return error code
 */
size_t readFileToBuf(FILE *fp, size_t *lenOfFile, char **txt);

/**
 * @brief return length of file in bytes
 *
 * @param fp - opened file
 * @param lenOfFile - length of file
 * @return error code
 */
int getLenOfFile(FILE *fp, size_t *lenOfFile);

/**
 * @brief counts lines in buffer
 * @param txt - buffer with text
 * @param lenOfFile - len of buffer
 * @return number of lines in buffer
 */
size_t countLines(const char *txt, size_t lenOfFile);

/**
 * @brief checks if code has correct compilation const and version
 *
 * @param buf buffer with code
 * @return error code
 */
size_t verifyCode(char **buf);

/**
 * @brief frees memory for struct code
 *
 * @param code
 */
void freeCode(Code *code);

/**
 * @brief calculates current day
 *
 * @return current week day
 */
int get_weekday();
#endif //STACK__UTILS_H
