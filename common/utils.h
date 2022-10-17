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

const uint8_t CMD_MASK = 0x1F;
const uint8_t ARG_MASK = 0xE0;

const uint8_t RAM_MASK = 0x80;
const uint8_t REG_MASK = 0x40;
const uint8_t IMM_MASK = 0x20;
const int EPS = 8;
const float SQUEEZE = 1.5;

enum UTILS_ERRORS
{
    NO_ERRORS                     = 0,
    CANT_GET_FILE_INFO            = 1 << 1,
    CANT_ALLOCATE_MEMORY_FOR_FILE = 1 << 2,
    NOT_EXECUTABLE_FILE           = 1 << 3,
    INCORRECT_VERSION             = 1 << 4,
};

const size_t  COMPILATION_CONST = 0xDEADF00D;
const size_t  VERSION_CONST     = 10;
const char    REGS_NAMES[5][4]  = {"r0x", "rax", "rbx", "rcx", "rdx"};
const uint8_t BUFFER_SIZE       = 128;

struct Code
{
    uint8_t *code;
    size_t len;
};

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
size_t readFileToBuf(FILE *fp, size_t *lenOfFile, char **txt);

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
 * @brief Reads code from binary file
 *
 * @param fp file to read from
 * @param code struct to read code
 * @return
 */
size_t readCode(FILE *fp, Code *code);

int get_weekday();
#endif //STACK__UTILS_H
