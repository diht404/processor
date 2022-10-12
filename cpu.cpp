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

#define applyOperation(cmd_case, operation)                   \
(command == (cmd_case))                                       \
    {                                                         \
        int firstValue = 0;                                   \
        int secondValue = 0;                                  \
                                                              \
        error = stackPop(cpu->stack, &secondValue);           \
        if (error)                                            \
            return error;                                     \
                                                              \
        error = stackPop(cpu->stack, &firstValue);            \
        if (error)                                            \
            return error;                                     \
        error |= stackPush(cpu->stack,                        \
                           firstValue operation secondValue); \
        if (error)                                            \
            return error;                                     \
                                                              \
        cpu->ip++;                                            \
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
        uint8_t command = cpu->code->code[cpu->ip] & CMD_MASK;
        uint8_t args = cpu->code->code[cpu->ip] & ARG_MASK;

        if (command == COMMAND_CODES::CMD_PUSH)
        {
            cpu->ip++;
            int arg = 0;

            int command_arg = *(int *) (cpu->code->code + cpu->ip);

            if (args & IMM_MASK)
                arg += command_arg;

            if (args & REG_MASK)
                arg += cpu->regs[command_arg];

            if (args & RAM_MASK)
                arg = cpu->RAM[command_arg];

            stackError |= stackPush(cpu->stack, arg);
            if (stackError)
                return stackError;

            cpu->ip += sizeof(int);
        }

        else if applyOperation(COMMAND_CODES::CMD_ADD, +)
        else if applyOperation(COMMAND_CODES::CMD_SUB, -)
        else if applyOperation(COMMAND_CODES::CMD_MUL, *)
        else if applyOperation(COMMAND_CODES::CMD_DIV, /)

#undef applyOperation

        else if (command == COMMAND_CODES::CMD_OUT)
        {
            int value = 0;

            stackError |= stackPop(cpu->stack, &value);
            if (stackError)
                return stackError;
            printf("ANSWER = %d\n", value);
            cpu->ip++;
        }
        else if (command == COMMAND_CODES::CMD_HLT)
        {
            return stackError;
        }
        else if (command == COMMAND_CODES::CMD_DUMP)
        {
            stackDump(cpu->stack, &cpu->stack->info, stackError);
            processorDump(stdout, cpu);
            cpu->ip++;
        }
        else if (command == COMMAND_CODES::CMD_IN)
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
        else if (command == COMMAND_CODES::CMD_POP)
        {
            cpu->ip++;
            int arg = 0;

            int command_arg = *(int *) (cpu->code->code + cpu->ip);

            if (args & REG_MASK)
            {
                stackError |= stackPop(cpu->stack, &arg);
                if (stackError)
                    return stackError;

                cpu->regs[command_arg] = arg;
            }

            if (args & RAM_MASK)
            {
                stackError |= stackPop(cpu->stack, &arg);
                if (stackError)
                    return stackError;

                cpu->RAM[command_arg] = arg;
            }
            cpu->ip += sizeof(int);
        }
        else if (command == COMMAND_CODES::CMD_JMP)
        {
            cpu->ip++;
            int arg = 0;

            int command_arg = *(int *) (cpu->code->code + cpu->ip);

            if (args & IMM_MASK)
                arg += command_arg;
            if (args & REG_MASK)
                arg += cpu->regs[command_arg];
            if (args & RAM_MASK)
                arg = cpu->RAM[arg];

            cpu->ip = arg;
        }
        else
        {
            fprintf(stderr, "CPU_UNKNOWN_COMMAND\n");
            return CPU_ERRORS::CPU_UNKNOWN_COMMAND;
        }
    }
    return CPU_ERRORS::CPU_NO_ERRORS;
}

int main(int argc, char *argv[])
{
    size_t error = NO_ERRORS;
    const char *input_filename = "data.code";

    if (argc == 2)
    {
        input_filename = argv[1];
    }
    if (argc > 2)
    {
        return -1;
    }
    setbuf(stdout, NULL);
    FILE *fp = fopen(input_filename, "rb");

    Stack stack = {};
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
    return 0;
}
