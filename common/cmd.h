#include "config.h"

#define NEXT_COMMAND \
    cpu->ip++;       \

#define POP(var)                       \
    error = stackPop(cpu->stack, var); \
    if (error)                         \
        return error;

#define CALL_POP(var)                       \
    error = stackPop(cpu->call_stack, var); \
    if (error)                              \
        return error;

#define ram  \
    cpu->RAM \

#define REGS  \
    cpu->regs \

#define SET_ARG   \
    cpu->ip = arg;\

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
        arg += REGS[command_arg];                          \
    if (args & RAM_MASK)                                   \
    {                                                      \
        sleep(0);                                          \
        arg = ram[arg];                                    \
    }

#define ARG_COMMAND_STEP()                                 \
    NEXT_COMMAND                                           \
    int arg = 0;                                           \
    int command_arg = *(int *) (cpu->code->code + cpu->ip);\

#define ARG_STEP()          \
    cpu->ip += sizeof(int);

#define SHOW_RAM_DATA                                        \
    printf("\n");                                            \
    for (int y = 0; y < size; y++)                           \
    {                                                        \
        for (int x = 0; x < size; x++)                       \
        {                                                    \
            printf("%s", ram[size * y + x]? "* ": ". ");     \
        }                                                    \
        printf("\n");                                        \
    }

DEF_CMD(HLT, 0, 0, {
    return error;
})

DEF_CMD(PUSH, 1, 1, {
    ARG_COMMAND_STEP()
    if (args & IMM_MASK)
        arg += command_arg * precision;
    if (args & REG_MASK)
        arg += REGS[command_arg];
    if (args & RAM_MASK)
    {
        sleep(0);
        arg = ram[arg / precision];
    }
    PUSH_VALUE(arg)
    ARG_STEP()
})

DEF_CMD(ADD, 2, 0, {
    POP_TWO()
    PUSH_VALUE(firstValue + secondValue);
    NEXT_COMMAND
})

DEF_CMD(MUL, 3, 0, {
    POP_TWO()
    PUSH_VALUE(firstValue * secondValue / precision);
    NEXT_COMMAND
})

DEF_CMD(SUB, 4, 0, {
    POP_TWO()
    PUSH_VALUE(firstValue - secondValue);
    NEXT_COMMAND
})

DEF_CMD(DIV, 5, 0, {
    POP_TWO()
    if (secondValue == 0)
        return DIVISION_BY_ZER0;
    PUSH_VALUE(precision * firstValue / secondValue);
    NEXT_COMMAND
})

DEF_CMD(OUT, 6, 0, {
    int value = 0;

    POP(&value);
    float output = (float)value;
    printf("ANSWER = %lg\n", output / precision);
    NEXT_COMMAND
})

DEF_CMD(DUMP, 7, 0, {
    stackDump(cpu->stack, &cpu->stack->info, error);
    stackDump(cpu->call_stack, &cpu->call_stack->info, error);
    processorDump(stderr, cpu);
    NEXT_COMMAND
})

DEF_CMD(IN, 8, 0, {
    int value = 0;

    printf("Enter number: \n");

    if (!scanf("%d", &value))
        return CPU_ERRORS::CPU_READ_FROM_CONSOLE_FAILED;

    PUSH_VALUE(value * precision);
    NEXT_COMMAND
})

DEF_CMD(POP, 9, 1, {
    ARG_COMMAND_STEP()

    if (args & REG_MASK)
    {
        POP(&arg)
        REGS[command_arg] = arg;
    }

    if (args & RAM_MASK)
    {
        POP(&arg)
        sleep(0);
        ram[command_arg] = arg;
    }
    ARG_STEP()
})

DEF_CMD(JMP, 10, 1, {
    ARG_COMMAND_STEP()
    GET_ARG()
    SET_ARG
})

#define CONDITION_JMP(operation)          \
{                                         \
    ARG_COMMAND_STEP()                    \
    POP_TWO()                             \
                                          \
    if (firstValue operation secondValue) \
    {                                     \
        GET_ARG()                         \
        SET_ARG                           \
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
    SET_ARG
})

DEF_CMD(RET, 18, 0, {
    NEXT_COMMAND
    int arg = 0;
    CALL_POP(&arg)
    SET_ARG
})

DEF_CMD(NO_SOLS, 19, 0, {
    printf("NO SOLUTIONS\n");
    NEXT_COMMAND
})

DEF_CMD(ANY_NUM, 20, 0, {
    printf("ANY NUMBER\n\n");
    NEXT_COMMAND
})

DEF_CMD(SQRT, 21, 0, {
    int value = 0;
    POP(&value)
    PUSH_VALUE((int)sqrt(value * precision))
    NEXT_COMMAND
})

DEF_CMD(CIRCLE_PICTURE, 22, 0, {
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
                    ram[size * y + x] = 1;
            else
                ram[size * y + x] = 0;
        }
    }
    SHOW_RAM_DATA
    NEXT_COMMAND
})

DEF_CMD(SQUARE_PICTURE, 23, 0, {
    int size = sqrt(cpu->vram_size);//rax
    int size_of_square = 4;//rbx

    for (int y = 0; y < size; y++)//rcx
    {
        for (int x = 0; x < size; x++)//rdx
        {
            if (x % size_of_square > 0 and y % size_of_square > 0)
                ram[size * y + x] = 1;
            else
                ram[size * y + x] = 0;
        }
    }
    SHOW_RAM_DATA
    NEXT_COMMAND
})

DEF_CMD(SHOW_RAM, 24, 0, {
    int size = sqrt(cpu->vram_size);
    SHOW_RAM_DATA
    NEXT_COMMAND
})

DEF_CMD(MOD, 25, 0, {
    POP_TWO()
    PUSH_VALUE(firstValue % secondValue);
    NEXT_COMMAND
})

DEF_CMD(SET_RAM, 26, 0, {
    POP_TWO()
    ram[firstValue / precision] = secondValue / precision;
    NEXT_COMMAND
})

DEF_CMD(JMPM, 27, 1, {
    ARG_COMMAND_STEP()
    GET_ARG()
    printf("WEEKDAY %d\n", get_weekday());
    if (get_weekday() == 1)
        SET_ARG
    else
        ARG_STEP()
})