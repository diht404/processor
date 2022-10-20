#ifndef STACK_COMMON_CONSTS_H
#define STACK_COMMON_CONSTS_H

const uint8_t CMD_MASK = 0x1F;
const uint8_t ARG_MASK = 0xE0;

const uint8_t RAM_MASK = 0x80;
const uint8_t REG_MASK = 0x40;
const uint8_t IMM_MASK = 0x20;
const int EPS = 8;
const float SQUEEZE = 1.5;

const size_t COMPILATION_CONST = 0xDEADF00D;
const size_t VERSION_CONST = 10;
const char REGS_NAMES[5][4] = {"ded", "rax", "rbx", "rcx", "rdx"};
const uint8_t BUFFER_SIZE = 128;

#endif //STACK_COMMON_CONSTS_H
