#ifndef STACK__DISASSEMBLER_H
#define STACK__DISASSEMBLER_H

#include "../common/utils.h"

enum DISASSEMBLER_ERRORS
{
    UNKNOWN_COMMAND_CODE    = 1 << 6,
    UNKNOWN_REG             = 1 << 7,
    CODE_IS_NULLPTR         = 1 << 8,
    FILE_IS_NULLPTR         = 1 << 9,
    COMMAND_NAME_IS_NULLPTR = 1 << 10,
    IP_IS_NULLPTR           = 1 << 11,
};

/**
 * @brief prints to file commands with arguments
 *
 * @param code array with code
 * @param fp file to write
 * @param command_name name of command to write
 * @param ip pointer to command
 * @return
 */
size_t printArg(Code *code, FILE *fp, const char *command_name, size_t *ip);

/**
 * @brief disassembles code and writes to file
 *
 * @param code array with code
 * @param fp file to write
 * @return
 */
size_t disassemle(Code *code, FILE *fp);

#endif //STACK__DISASSEMBLER_H
