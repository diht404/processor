#include "cpu.h"

void applyOperator(Stack *stack, char operation, size_t *error)
{

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
        case '+':*error |= stackPush(stack, firstValue + secondValue);
            break;
        case '-':*error |= stackPush(stack, firstValue - secondValue);
            break;
        case '*':*error |= stackPush(stack, firstValue * secondValue);
            break;
        case '/':*error |= stackPush(stack, firstValue / secondValue);
            break;
        default:*error |= CPU_UNKNOWN_COMMAND;
    }
}

size_t run(Code *code, Stack *stack)
{
    size_t ip = 0;
    size_t stackError = 0;

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
//          TODO: processor dump
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
