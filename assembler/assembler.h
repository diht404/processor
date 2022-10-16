#ifndef STACK__ASSEMBLER_H
#define STACK__ASSEMBLER_H

#include "../utils.h"

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
    char **lines = nullptr;
    size_t length = 0;
};

struct NamesTable
{
    char names_table[BUFFER_SIZE][BUFFER_SIZE] = {};
    int positions[BUFFER_SIZE] = {};
};

#define reg_compile(cmd_arg, reg_name, number) \
    if (strcasecmp(buffer, (reg_name)) == 0)   \
    {                                          \
        **code = (cmd_arg) | REG_MASK;         \
        (*lenOfCode)++;                        \
        (*code)++;                             \
        **(int **) code = (number);            \
        *lenOfCode += sizeof(int);             \
        *code += sizeof(int);                  \
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

/**
 * @brief skips spaces in code
 *
 * @param program struct with code
 * @param line line of code
 * @param commandSize len of command in symbols
 */
void skipSpaces(Program *program, size_t line, int *commandSize);

/**
 * @brief process code if there is [] in line of code
 *
 * @param program struct with code
 * @param code array with code
 * @param commandSize len of command in symbols
 * @param buffer buffer for storing code extracted from []
 * @param line line of code
 * @param error error code
 */
void detectBrackets(Program *program,
                    uint8_t *code,
                    int commandSize,
                    char *buffer,
                    size_t line,
                    size_t *error);

void detectPlus();

/**
 * @brief process push args
 *
 * @param code array with code
 * @param command command to process
 * @param buffer buffer for storing code extracted from []
 * @param lenOfCode length of array with code
 * @param value value to push
 * @param error error code
 */
void processArgs(uint8_t **code,
                 int command_code,
                 char *buffer,
                 int *lenOfCode,
                 int value,
                 size_t *error);

/**
 * @brief puts args to code
 *
 * @param program struct with code
 * @param line line index of code
 * @param code array with code
 * @param commandSize length of command
 * @param lenOfCode length of array with code
 * @param command_code code of command
 * @param table names table
 * @param error error code
 */
void putArgs(Program *program,
             size_t line,
             uint8_t **code,
             int *commandSize,
             int *lenOfCode,
             int command_code,
             NamesTable *table,
             size_t *error);

/**
 * @brief Compiles code
 *
 * @param program array with program to compile
 * @param table names table
 * @param error error code
 * @return compiled code
 */
uint8_t *compile(Program *program, NamesTable *table, size_t *error);

/**
 * @brief fills names table and search index of label in table
 *
 * @param names_table table with names of labels
 * @param name name of label
 * @param ip index of label
 */
void fillNameTable(NamesTable *table,
                   char name[BUFFER_SIZE],
                   int ip);

/**
 * @brief finds id of label in names table
 *
 * @param names_table table with names of labels
 * @param name name of label
 * @return index of label in name table or -1 if not success
 */
int getIpFromTable(NamesTable *table,
                   char name[BUFFER_SIZE]);

/**
 * @brief compiles code with names table
 *
 * @param program array with program to compile
 * @param error error code
 * @return compiled code
 */
uint8_t *compileWithNamesTable(Program *program,
                               size_t *error);

/**
 * @brief Saves array with compiled code to file
 *
 * @param code code to write to file
 * @param filename name of file to write
 * @return error code
 */
size_t saveFile(uint8_t *code, const char *filename);

#endif //STACK__ASSEMBLER_H
