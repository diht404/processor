#ifndef STACK__DISASSEMBLER_H
#define STACK__DISASSEMBLER_H

#include "utils.h"

enum DISASSEMBLER_ERRORS
{
    UNKNOWN_COMMAND = 1 << 6,
    UNKNOWN_REG     = 1 << 7,
};

size_t disassemle(Code *code, FILE *fp);

#endif //STACK__DISASSEMBLER_H
