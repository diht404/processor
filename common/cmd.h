#include "config.h"

#define POP(var)                       \
    error = stackPop(cpu->stack, var); \
    if (error)                         \
        return error;

#define CALL_POP(var)                       \
    error = stackPop(cpu->call_stack, var); \
    if (error)                              \
        return error;

#define POP_TWO()       \
    int firstValue  = 0;\
    int secondValue = 0;\
                        \
    POP(&secondValue);  \
    POP(&firstValue);   \

#define PUSH_VALUE(value)                 \
    error = stackPush(cpu->stack, value); \
    if (error)                            \
        return error;

#define CALL_PUSH(value)                       \
    error = stackPush(cpu->call_stack, value); \
    if (error)                                 \
        return error;

#define GET_ARG()                                          \
    if (args & IMM_MASK)                                   \
        arg += command_arg;                                \
    if (args & REG_MASK)                                   \
        arg += cpu->regs[command_arg];                     \
    if (args & RAM_MASK)                                   \
    {                                                      \
        sleep(0);                                          \
        arg = cpu->RAM[arg];                               \
    }

#define ARG_COMMAND_STEP()                                 \
    cpu->ip++;                                             \
    int arg = 0;                                           \
    int command_arg = *(int *) (cpu->code->code + cpu->ip);\

#define ARG_STEP()          \
    cpu->ip += sizeof(int);

DEF_CMD(HLT, 0, 0, {
    return error;
})

DEF_CMD(PUSH, 1, 1, {
    ARG_COMMAND_STEP()
    if (args & IMM_MASK)
        arg += command_arg * precision;
    if (args & REG_MASK)
        arg += cpu->regs[command_arg];
    if (args & RAM_MASK)
    {
        sleep(0);
        arg = cpu->RAM[arg / precision];
    }
    PUSH_VALUE(arg)
    ARG_STEP()
})

DEF_CMD(ADD, 2, 0, {
    POP_TWO()
    PUSH_VALUE(firstValue + secondValue);
    cpu->ip++;
})

DEF_CMD(MUL, 3, 0, {
    POP_TWO()
    PUSH_VALUE(firstValue * secondValue / precision);
    cpu->ip++;
})

DEF_CMD(SUB, 4, 0, {
    POP_TWO()
    PUSH_VALUE(firstValue - secondValue);
    cpu->ip++;
})

DEF_CMD(DIV, 5, 0, {
    POP_TWO()
    if (secondValue == 0)
        return DIVISION_BY_ZER0;
    PUSH_VALUE(precision * firstValue / secondValue);
    cpu->ip++;
})

DEF_CMD(OUT, 6, 0, {
    int value = 0;

    POP(&value);
    float output = (float)value;
    printf("ANSWER = %lg\n", output / precision);
    cpu->ip++;
})

DEF_CMD(DUMP, 7, 0, {
    stackDump(cpu->stack, &cpu->stack->info, error);
    stackDump(cpu->call_stack, &cpu->call_stack->info, error);
    processorDump(stderr, cpu);
    cpu->ip++;
})

DEF_CMD(IN, 8, 0, {
    int value = 0;

    printf("Enter number: \n");

    if (!scanf("%d", &value))
        return CPU_ERRORS::CPU_READ_FROM_CONSOLE_FAILED;

    PUSH_VALUE(value * precision);

    cpu->ip++;
})

DEF_CMD(POP, 9, 1, {
    ARG_COMMAND_STEP()

    if (args & REG_MASK)
    {
        POP(&arg)
        cpu->regs[command_arg] = arg;
    }

    if (args & RAM_MASK)
    {
        POP(&arg)
        sleep(0);
        cpu->RAM[command_arg] = arg;
    }
    ARG_STEP()
})

DEF_CMD(JMP, 10, 1, {
    ARG_COMMAND_STEP()
    GET_ARG()
    cpu->ip = arg;
})

#define CONDITION_JMP(operation)          \
{                                         \
    ARG_COMMAND_STEP()                    \
    POP_TWO()                             \
                                          \
    if (firstValue operation secondValue) \
    {                                     \
        GET_ARG()                         \
        cpu->ip = arg;                    \
    }                                     \
    else                                  \
        ARG_STEP()                        \
}

DEF_CMD(JA, 11, 1, CONDITION_JMP(>))
DEF_CMD(JAE, 12, 1, CONDITION_JMP(>=))
DEF_CMD(JB, 13, 1, CONDITION_JMP(<))
DEF_CMD(JBE, 14, 1, CONDITION_JMP(<=))
DEF_CMD(JE, 15, 1, CONDITION_JMP(==))
DEF_CMD(JNE, 16, 1, CONDITION_JMP(!=))

DEF_CMD(CALL, 17, 1, {
    ARG_COMMAND_STEP()
    GET_ARG()
    CALL_PUSH(cpu->ip + sizeof(int));
    cpu->ip = arg;
})

DEF_CMD(RET, 18, 0, {
    cpu->ip++;
    int arg = 0;
    CALL_POP(&arg)
    cpu->ip = arg;
})

DEF_CMD(NO_SOLS, 19, 0, {
    printf("NO SOLUTIONS\n");
    cpu->ip++;
})

DEF_CMD(ANY_NUM, 20, 0, {
    printf("ANY NUMBER\n\n");
    cpu->ip++;
})

DEF_CMD(SQRT, 21, 0, {
    int value = 0;
    POP(&value)
    PUSH_VALUE((int)sqrt(value * precision))
    cpu->ip++;
})

DEF_CMD(PICTURE, 22, 0, {
    int size = sqrt(cpu->vram_size);
    int radius = size / 2;

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            if (abs((x - radius) * (x - radius) +
                    (y - radius) * (y - radius) -
                    radius * radius / SQUEEZE
                    ) < EPS)
                cpu->RAM[size * y + x] = 1;
        }
    }

    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            printf("%s", cpu->RAM[size * y + x]? "***": "...");
        }
        printf("\n");
    }
    cpu->ip++;
})
