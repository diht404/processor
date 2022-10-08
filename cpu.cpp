#include "cpu.h"

void processorDump(FILE *fp, CPU *cpu)
{
    if (fp == nullptr)
        fp = stderr;

    if (cpu == nullptr)
        return;

    for (size_t i = 0; i < cpu->code->len; i++)
    {
        if (cpu->ip == i)
        {
            fprintf(fp, "{{{ %d }}} ", cpu->code->code[i]);
            continue;
        }
        fprintf(fp, "%d ", cpu->code->code[i]);
    }
    fprintf(fp, "\n");
}

#define applyOperation(cmd_case, operation)                               \
else if (cpu->code->code[cpu->ip] == (cmd_case))                          \
    {                                                                     \
        int firstValue = 0;                                               \
        int secondValue = 0;                                              \
        error = stackPop(cpu->stack, &secondValue);                       \
        if (error)                                                        \
            return error;                                                 \
        error = stackPop(cpu->stack, &firstValue);                        \
        if (error)                                                        \
            return error;                                                 \
        error |= stackPush(cpu->stack, firstValue operation secondValue); \
        if (error)                                                        \
            return error;                                                 \
        cpu->ip++;                                                        \
    }

size_t run(CPU *cpu)
{
    assert(cpu != nullptr);

    Stack stack_run = {};
    size_t stackError = 0;

    if (cpu->stack == nullptr)
    {
        cpu->stack = &stack_run;
        stackCtor(cpu->stack, 1, &stackError)
    }

    size_t error = NO_ERRORS;

    while (cpu->ip < cpu->code->len)
    {
        if (cpu->code->code[cpu->ip] == COMMAND_CODES::PUSH)
        {
            cpu->ip++;
            stackError |= stackPush(cpu->stack, *(int *) (cpu->code->code + cpu->ip));
            cpu->ip += sizeof(int);

            if (stackError)
                return stackError;
        }

        applyOperation(COMMAND_CODES::ADD, +)
        applyOperation(COMMAND_CODES::SUB, -)
        applyOperation(COMMAND_CODES::MUL, *)
        applyOperation(COMMAND_CODES::DIV, /)
#undef applyOperation

        else if (cpu->code->code[cpu->ip] == COMMAND_CODES::OUT)
        {
            int value = 0;

            stackError |= stackPop(cpu->stack, &value);
            if (stackError)
                return stackError;

            printf("ANSWER = %d\n", value);
            cpu->ip++;
        }
        else if (cpu->code->code[cpu->ip] == COMMAND_CODES::HLT)
        {
            return stackError;
        }
        else if (cpu->code->code[cpu->ip] == COMMAND_CODES::DUMP)
        {
            stackDump(cpu->stack, &cpu->stack->info, stackError);
            processorDump(stdout, cpu);
            cpu->ip++;
        }
        else if (cpu->code->code[cpu->ip] == COMMAND_CODES::IN)
        {
            int value = 0;
            printf("Enter number: \n");
            if (!scanf("%d", &value))
                return CPU_ERRORS::CPU_READ_FROM_CONSOLE_FAILED;
            stackError |= stackPush(cpu->stack, value);
            if (stackError)
                return stackError;
            cpu->ip++;
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
    CPU cpu = {&code, &stack};

    error = readCode(fp, &code);

    if (error)
    {
        printf("ERROR read: %zu\n", error);
    }

    error = run(&cpu);
    if (error)
    {
        printf("ERROR run: %zu\n", error);
        printf("Stack size: %zu\n", stack.size);
    }
    fclose(fp);
    printf("\nStopping CPU...");
    return 0;
}
