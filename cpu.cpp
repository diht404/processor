#include "cpu.h"

void applyOperator(Stack *stack, char operation, size_t *error)
{
    assert(stack != nullptr);
    size_t no_errors = 0;

    if (error == nullptr)
    {
        error = &no_errors;
    }

    int firstValue = 0;
    int secondValue = 0;

    *error |= stackPop(stack, &secondValue);
    if (*error)
        return;

    *error |= stackPop(stack, &firstValue);
    if (*error)
        return;

    switch (operation)
    {
        case '+':
        {
            *error |= stackPush(stack, firstValue + secondValue);
            break;
        }
        case '-':
        {
            *error |= stackPush(stack, firstValue - secondValue);
            break;
        }
        case '*':
        {
            *error |= stackPush(stack, firstValue * secondValue);
            break;
        }
        case '/':
        {
            *error |= stackPush(stack, firstValue / secondValue);
            break;
        }
        default:
        {
            *error |= CPU_UNKNOWN_COMMAND;
            return;
        }
    }
}

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

    size_t ip = 0;
    while (ip < code->len)
    {
        if (code->code[ip] == COMMAND_CODES::PUSH)
        {
            ip++;
            stackError |= stackPush(stack, code->code[ip]);
            ip++;

            if (stackError)
                return stackError;
        }
        else if (code->code[ip] == COMMAND_CODES::ADD)
        {
            applyOperator(stack, '+', &stackError);
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::SUB)
        {
            applyOperator(stack, '-', &stackError);
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::MUL)
        {
            applyOperator(stack, '*', &stackError);
            ip++;
        }
        else if (code->code[ip] == COMMAND_CODES::DIV)
        {
            applyOperator(stack, '/', &stackError);
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
