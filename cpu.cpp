#include "utils.h"
#include "stack/stack.h"
#include "cpu.h"
#include "assembler.h"

size_t readCode(FILE *fp, Code *code)
{
    assert(fp != nullptr);
    assert(code != nullptr);

    size_t lenOfFile = 0;
    char *buf = nullptr;
    int error = readFileToBuf(fp, &lenOfFile, &buf);

    if (error)
        return error;

    size_t compilationConst = *(size_t *) (buf);

    if (compilationConst != COMPILATION_CONST)
        return CPU_NOT_EXECUTABLE_FILE;

    size_t version = *((size_t *) (buf) + 1);

    if (version != VERSION)
        return CPU_INCORRECT_VERSION;

    code->len = *((size_t *) (buf) + 2);

    code->code = (int *)((size_t *) (buf) + 3);
    return CPU_NO_ERRORS;
}

size_t run(Code *code, Stack *stack)
{
    size_t ip = 0;
    size_t stackError = 0;
    while (ip < code->len)
    {
//        printf("IP: %zu COMMAND: %d \n", ip, code->code[ip]);
        if (code->code[ip] == COMMAND_CODES::PUSH)
        {
            ip++;
            stackError |= stackPush(stack, code->code[ip]);
            if (stackError)
                return stackError;
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::ADD)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
                return stackError;

            stackError |= stackPop(stack, &firstValue);
            if (stackError)
                return stackError;

            stackError |= stackPush(stack, firstValue + secondValue);
            if (stackError)
                return stackError;
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::SUB)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
                return stackError;

            stackError |= stackPop(stack, &firstValue);
            if (stackError)
                return stackError;

            stackError |= stackPush(stack, firstValue - secondValue);
            if (stackError)
                return stackError;
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::MUL)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
            {
                return stackError;
            }
            stackError |= stackPop(stack, &firstValue);
            if (stackError)
            {
                return stackError;
            }
            stackError |= stackPush(stack, firstValue * secondValue);
            if (stackError)
                return stackError;
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::DIV)
        {
            int firstValue = 0;
            int secondValue = 0;

            stackError |= stackPop(stack, &secondValue);
            if (stackError)
                return stackError;

            stackError |= stackPop(stack, &firstValue);
            if (stackError)
                return stackError;

            stackError |= stackPush(stack, firstValue / secondValue);
            if (stackError)
                return stackError;
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::OUT)
        {
            int value = 0;

            stackError |= stackPop(stack, &value);
            if (stackError)
                return stackError;

            printf("ANSWER = %d\n", value);
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::HLT)
        {
//            printf("COMMAND: %d \n", code->code[ip]);
            ip++;
            return stackError;
        }
        else if (code->code[ip] == COMMAND_CODES::DUMP)
        {
//          TODO: processor dump
            return stackError;
        }
        else if (code->code[ip] == COMMAND_CODES::IN)
        {
            int value = 0;
            if (!scanf("%d", &value))
                return CPU_ERRORS::CPU_READ_FAILED;
            stackError |= stackPush(stack, value);
            if (stackError)
                return stackError;
            ip++;
        }
        else
        {
            return CPU_ERRORS::CPU_UNKNOWN_COMMAND;
        }
    }
    return CPU_ERRORS::CPU_NO_ERRORS;
}

int main()
{
    FILE *fp = nullptr;
    openFile("1.code", "rb", &fp);

    Stack stack = {};
    size_t error = 0;
    stackCtor(&stack, 1, &error)

    Code code = {};

    error = readCode(fp, &code);
    error = run(&code, &stack);

    if (error)
    {
        printf("ERROR: %zu\n", error);
        printf("Stack size: %zu\n", stack.size);
    }
    fclose(fp);
}
