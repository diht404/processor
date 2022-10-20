#ifndef CPU__ASSEMBLER_H
#define CPU__ASSEMBLER_H

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
    TOKEN_PTR_IS_NULLPTR                       = 1 << 12,
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
 * @brief compiles code with names table
 *
 * @param program array with program to compile
 * @return error code
 */
size_t compileWithNamesTable(AsmProgram *program,
                             Code *code);

/**
 * @brief Compiles code
 *
 * @param program array with program to compile
 * @param code struct with code
 * @param error error code
 * @return error code
 */
size_t compile(AsmProgram *program, Code *code);

/**
 * @brief skips spaces in code
 *
 * @param program struct with code
 * @param line line of code
 * @param tokenPtr len of command in symbols
 */
void skipSpaces(AsmProgram *program, size_t line, int *tokenPtr);

/**
 * @brief fills names table and search index of label in table
 *
 * @param names_table table with names of labels
 * @param name name of label
 * @param ip index of label
 */
void fillNameTable(NameTable *nameTable,
                   char name[BUFFER_SIZE],
                   int ip);

/**
 * @brief puts args to code
 *
 * @param program struct with code
 * @param line line index of code
 * @param code array with code
 * @param commandSize length of command
 * @param lenOfCode length of array with code
 * @param command_code code of command
 * @return error code
 */
size_t putArgs(AsmProgram *program,
             size_t line,
             Code *code,
             int *commandSize,
             int *lenOfCode,
             int command_code);

/**
 * @brief process code if there is [] in line of code
 *
 * @param program struct with code
 * @param code array with code
 * @param commandSize len of command in symbols
 * @param buffer buffer for storing code extracted from []
 * @param line line of code
 * @return error code
 */
size_t detectBrackets(AsmProgram *program,
                    Code *code,
                    int commandSize,
                    char *buffer,
                    size_t line);

/**
 * @brief finds id of label in names nameTable
 *
 * @param names_table nameTable with names of labels
 * @param name name of label
 * @return index of label in name nameTable or -1 if not success
 */
int getIpFromTable(NameTable *nameTable,
                   char name[BUFFER_SIZE]);

/**
 * @brief process push args
 *
 * @param code array with code
 * @param command command to process
 * @param buffer buffer for storing code extracted from []
 * @param lenOfCode length of array with code
 * @param value value to push
 * @return error code
 */
size_t processArgs(Code *code,
                 int command_code,
                 char *buffer,
                 int *lenOfCode,
                 int value);

/**
 * @brief Adds info about code: the compilation const, version, and code length
 *
 * @param code array with code
 * @param lenOfCode length of code in bytes
 */
void addInfo(Code *code, int lenOfCode);

/**
 * @brief reads file to struct AsmProgram
 *
 * @param fp - opened file
 * @param program - struct AsmProgram with file
 * @return error code
 */
size_t readFile(FILE *fp, AsmProgram *program);

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
 * @brief process error
 *
 * @param error error code
 */
void handleAsmError(size_t error);

#endif //CPU__ASSEMBLER_H
