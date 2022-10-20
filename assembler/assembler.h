#ifndef STACK__ASSEMBLER_H
#define STACK__ASSEMBLER_H

#include "../common/utils.h"

enum ASSEMBLER_ERRORS
{
    ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_PROGRAM = 1 << 4,
    ASSEMBLER_CANT_ALLOCATE_MEMORY_FOR_STRINGS = 1 << 5,
    ASSEMBLER_COMPILATION_FAILED               = 1 << 6,
    ASSEMBLER_CANT_SHRINK_TO_FIT               = 1 << 7,
    INCORRECT_BRACKETS                         = 1 << 8,
    CODE_IS_NULLPTR                            = 1 << 9,
    BUFFER_IS_NULLPTR                          = 1 << 10,
    PROGRAM_IS_NULLPTR                         = 1 << 11,
    COMMAND_SIZE_IS_NULLPTR                    = 1 << 12,
    LEN_OF_CODE_IS_NULLPTR                     = 1 << 13,
    NAME_TABLE_IS_NULLPTR                      = 1 << 14,
    FILENAME_IS_NULLPTR                        = 1 << 15,
    FILE_IS_NULLPTR                            = 1 << 16,
    HEADER_IS_NULLPTR                          = 1 << 17,
};

/**
 * @brief Struct for storing array of strings, its' length and pointer
 * to memory block with not sorted text(array of string)
 */
struct AsmProgram
{
    char **lines = nullptr;
    size_t length = 0;
};


#define reg_compile(cmd_arg, reg_name, number) \
    if (strcasecmp(buffer, (reg_name)) == 0)   \
    {                                          \
        *code->code = (cmd_arg) | REG_MASK;    \
        (*lenOfCode)++;                        \
        code->code++;                          \
        *(int *) code->code = (number);        \
        *lenOfCode += sizeof(int);             \
        code->code += sizeof(int);             \
    }

/**
 * @brief reads file to struct AsmProgram
 *
 * @param fp - opened file
 * @param program - struct AsmProgram with file
 * @return error code
 */
size_t readFile(FILE *fp, AsmProgram *program);

/**
 * @brief skips spaces in code
 *
 * @param program struct with code
 * @param line line of code
 * @param commandSize len of command in symbols
 */
void skipSpaces(AsmProgram *program, size_t line, int *commandSize);

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
void detectBrackets(AsmProgram *program,
                    Code *code,
                    int commandSize,
                    char *buffer,
                    size_t line,
                    size_t *error);

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
void processArgs(Code *code,
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
void putArgs(AsmProgram *program,
             size_t line,
             Code *code,
             int *commandSize,
             int *lenOfCode,
             int command_code,
             size_t *error);

/**
 * @brief compiles code with names table
 *
 * @param program array with program to compile
 * @param error error code
 */
void compileWithNamesTable(AsmProgram *program,
                           Code *code,
                           size_t *error);

/**
 * @brief Compiles code
 *
 * @param program array with program to compile
 * @param code struct with code
 * @param error error code
 * @return compiled code
 */
void compile(AsmProgram *program, Code *code, size_t *error);

/**
 * @brief fills names table and search index of label in table
 *
 * @param names_table table with names of labels
 * @param name name of label
 * @param ip index of label
 */
void fillNameTable(Code *code,
                   char name[BUFFER_SIZE],
                   int ip);

/**
 * @brief finds id of label in names table
 *
 * @param names_table table with names of labels
 * @param name name of label
 * @return index of label in name table or -1 if not success
 */
int getIpFromTable(NameTable *table,
                   char name[BUFFER_SIZE]);

/**
 * @brief Saves array with compiled code to file
 *
 * @param code code to write to file
 * @param filename name of file to write
 * @return error code
 */
size_t saveFile(Code *code, const char *filename);

/**
 * @brief saves header of file
 *
 * @param header header to save
 * @param fp file to save to
 * @return
 */
size_t saveHeader(CodeHeader *header, FILE *fp);

/**
 * @brief Adds info about code: the compilation const, version, and code length
 *
 * @param code array with code
 * @param lenOfCode length of code in bytes
 */
void addInfo(Code *code, int lenOfCode);

/**
 * @brief process error
 *
 * @param error error code
 */
void handleAsmError(size_t error);

#endif //STACK__ASSEMBLER_H
