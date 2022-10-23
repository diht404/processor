#ifndef CPU_COMMON_DSL_H
#define CPU_COMMON_DSL_H

#define NEXT_COMMAND                                           \
    {                                                          \
        cpu->ip++;                                             \
    }                                                          \

#define POP(var)                                               \
    {                                                          \
        error = stackPop(cpu->stack, var);                     \
        if (error)                                             \
            return error;                                      \
    }

#define CALL_POP(var)                                          \
    {                                                          \
        error = stackPop(cpu->call_stack, var);                \
        if (error)                                             \
            return error;                                      \
    }

#define RAM_MEM                                                \
    (cpu->RAM)                                                 \

#define REGS                                                   \
    (cpu->regs)                                                \

#define SET_ARG                                                \
    {                                                          \
        cpu->ip = arg;                                         \
    }

#define POP_TWO(firstValue, secondValue)                       \
    {                                                          \
        POP(secondValue);                                     \
        POP(firstValue);                                      \
    }

#define PUSH_VALUE(value)                                      \
    {                                                          \
        error = stackPush(cpu->stack, value);                  \
        if (error)                                             \
            return error;                                      \
    }

#define CALL_PUSH(value)                                       \
    {                                                          \
        error = stackPush(cpu->call_stack, value);             \
        if (error)                                             \
            return error;                                      \
    }

#define GET_ARG()                                              \
    {                                                          \
        if (args & IMM_MASK)                                   \
            arg += command_arg;                                \
        if (args & REG_MASK)                                   \
            arg += REGS[command_arg];                          \
        if (args & RAM_MASK)                                   \
        {                                                      \
            sleep(0);                                          \
            arg = RAM_MEM[arg];                                \
        }                                                      \
    }

#define ARG_COMMAND_STEP()                                     \
    {                                                          \
        NEXT_COMMAND                                           \
        command_arg = *(int *) (cpu->code->code + cpu->ip);    \
    }

#define ARG_STEP()                                             \
    {                                                          \
        cpu->ip += sizeof(int);                                \
    }

#endif //CPU_COMMON_DSL_H
