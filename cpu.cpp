#include "cpu.h"

void processorDump(FILE *fp, Code *code, size_t ip)
{
    if (fp == nullptr)
        fp = stderr;

    if (code == nullptr)
        return;

    for (size_t i = 0; i < code->len; i++)
    {
        if (ip == i)
        {
            fprintf(fp, "{{{ %d }}} ", code->code[i]);
            continue;
        }
        fprintf(fp, "%d ", code->code[i]);
    }
    fprintf(fp, "\n");
}

#define applyOperation(cmd_case, operation)                          \
else if (code->code[ip] == (cmd_case))                               \
    {                                                                \
        int firstValue = 0;                                          \
        int secondValue = 0;                                         \
        error = stackPop(stack, &secondValue);                       \
        if (error)                                                   \
            return error;                                            \
        error = stackPop(stack, &firstValue);                        \
        if (error)                                                   \
            return error;                                            \
        error |= stackPush(stack, firstValue operation secondValue); \
        if (error)                                                   \
            return error;                                            \
        ip++;                                                        \
    }

size_t run(Code *code, Stack *stack)
{
    assert(code != nullptr);
    Stack stack_run = {};
    size_t stackError = 0;

    if (stack == nullptr)
    {
        stack = &stack_run;
        stackCtor(stack, 1, &stackError)
    }

    size_t error = NO_ERRORS;

    size_t ip = 0;
    while (ip < code->len)
    {
        if (code->code[ip] == COMMAND_CODES::PUSH)
        {
            ip++;
            stackError |= stackPush(stack, *(int *) (code->code + ip));
            ip += sizeof(int);

            if (stackError)
                return stackError;
        }

        applyOperation(COMMAND_CODES::ADD, +)
        applyOperation(COMMAND_CODES::SUB, -)
        applyOperation(COMMAND_CODES::MUL, *)
        applyOperation(COMMAND_CODES::DIV, /)
#undef applyOperation

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
            return stackError;
        }
        else if (code->code[ip] == COMMAND_CODES::DUMP)
        {
            stackDump(stack, &stack->info, stackError);
            processorDump(stdout, code, ip);
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::IN)
        {
            int value = 0;
            printf("Enter number: \n");
            if (!scanf("%d", &value))
                return CPU_ERRORS::CPU_READ_FROM_CONSOLE_FAILED;
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
    setbuf(stdout, NULL);
    printf("Starting CPU...\n\n");
    FILE *fp = fopen("data.code", "rb");

    Stack stack = {};
    size_t error = 0;
    stackCtor(&stack, 1, &error)

    Code code = {};

    error = readCode(fp, &code);

    if (error)
    {
        printf("ERROR read: %zu\n", error);
    }

    error = run(&code, &stack);
    if (error)
    {
        printf("ERROR run: %zu\n", error);
        printf("Stack size: %zu\n", stack.size);
    }
    fclose(fp);
    printf("\nStopping CPU...");
    return 0;
}
