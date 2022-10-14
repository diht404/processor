DEF_CMD(HLT, 0, 0, {
    return stackError;
})

DEF_CMD(PUSH, 1, 1, {
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
})

DEF_CMD(ADD, 2, 0, {
    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    error |= stackPush(cpu->stack,
                       firstValue + secondValue);
    if (error)
        return error;

    cpu->ip++;
})

DEF_CMD(MUL, 3, 0, {
    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    error |= stackPush(cpu->stack,
                       firstValue * secondValue);
    if (error)
        return error;

    cpu->ip++;
})

DEF_CMD(SUB, 4, 0, {
    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    error |= stackPush(cpu->stack,
                       firstValue - secondValue);
    if (error)
        return error;

    cpu->ip++;
})


DEF_CMD(DIV, 5, 0, {
    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    if (secondValue == 0)
        return DIVISION_BY_ZER0;

    error |= stackPush(cpu->stack,
                       firstValue / secondValue);
    if (error)
        return error;

    cpu->ip++;
})

DEF_CMD(OUT, 6, 0, {
    int value = 0;

    stackError |= stackPop(cpu->stack, &value);
    if (stackError)
        return stackError;

    printf("ANSWER = %d\n", value);
    cpu->ip++;
})

DEF_CMD(DUMP, 7, 0, {
    stackDump(cpu->stack, &cpu->stack->info, stackError);
    stackDump(cpu->call_stack, &cpu->call_stack->info, stackError);
    processorDump(stderr, cpu);
    cpu->ip++;
})

DEF_CMD(IN, 8, 0, {
    int value = 0;

    printf("Enter number: \n");

    if (!scanf("%d", &value))
        return CPU_ERRORS::CPU_READ_FROM_CONSOLE_FAILED;

    stackError |= stackPush(cpu->stack, value);
    if (stackError)
        return stackError;

    cpu->ip++;
})

DEF_CMD(POP, 9, 1, {
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
})

DEF_CMD(JMP, 10, 1, {
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
})

DEF_CMD(JA, 11, 1, {
    cpu->ip++;
    int arg = 0;

    int command_arg = *(int *) (cpu->code->code + cpu->ip);

    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    if (firstValue > secondValue)
    {
        if (args & IMM_MASK)
            arg += command_arg;
        if (args & REG_MASK)
            arg += cpu->regs[command_arg];
        if (args & RAM_MASK)
            arg = cpu->RAM[arg];

        cpu->ip = arg;
    }
    else
        cpu->ip++;
})

DEF_CMD(JAE, 12, 1, {
cpu->ip++;
int arg = 0;

int command_arg = *(int *) (cpu->code->code + cpu->ip);

int firstValue = 0;
int secondValue = 0;

error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    if (firstValue >= secondValue)
    {
        if (args & IMM_MASK)
            arg += command_arg;
        if (args & REG_MASK)
            arg += cpu->regs[command_arg];
        if (args & RAM_MASK)
            arg = cpu->RAM[arg];

        cpu->ip = arg;
    }
    else
        cpu->ip+=sizeof(int);
})

DEF_CMD(JB, 13, 1, {
    cpu->ip++;
    int arg = 0;

    int command_arg = *(int *) (cpu->code->code + cpu->ip);

    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    if (firstValue < secondValue)
    {
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
        cpu->ip += sizeof(int);
    }

})

DEF_CMD(JBE, 14, 1, {
    cpu->ip++;
    int arg = 0;

    int command_arg = *(int *) (cpu->code->code + cpu->ip);

    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    if (firstValue <= secondValue)
    {
        if (args & IMM_MASK)
            arg += command_arg;
        if (args & REG_MASK)
            arg += cpu->regs[command_arg];
        if (args & RAM_MASK)
            arg = cpu->RAM[arg];

        cpu->ip = arg;
    }
    else
        cpu->ip+=sizeof(int);
})

DEF_CMD(JE, 15, 1, {
    cpu->ip++;
    int arg = 0;

    int command_arg = *(int *) (cpu->code->code + cpu->ip);

    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;
//    fprintf(stderr, "%d %d\n", firstValue, secondValue);
    if (firstValue == secondValue)
    {
        if (args & IMM_MASK)
            arg += command_arg;
        if (args & REG_MASK)
            arg += cpu->regs[command_arg];
        if (args & RAM_MASK)
            arg = cpu->RAM[arg];

        cpu->ip = arg;
    }
    else
        cpu->ip+=sizeof(int);
})

DEF_CMD(JNE, 17, 1, {
    cpu->ip++;
    int arg = 0;

    int command_arg = *(int *) (cpu->code->code + cpu->ip);

    int firstValue = 0;
    int secondValue = 0;

    error = stackPop(cpu->stack, &secondValue);
    if (error)
        return error;

    error = stackPop(cpu->stack, &firstValue);
    if (error)
        return error;

    if (firstValue != secondValue)
    {
        if (args & IMM_MASK)
            arg += command_arg;
        if (args & REG_MASK)
            arg += cpu->regs[command_arg];
        if (args & RAM_MASK)
            arg = cpu->RAM[arg];

        cpu->ip = arg;
    }
    else
        cpu->ip+=sizeof(int);
})

DEF_CMD(call, 18, 1, {
//    fprintf(stderr, "ip = %d\n", cpu->ip);
    cpu->ip++;
    if (error)
        return error;
    int arg = 0;

    int command_arg = *(int *) (cpu->code->code + cpu->ip);

    if (args & IMM_MASK)
        arg += command_arg;
    if (args & REG_MASK)
        arg += cpu->regs[command_arg];
    if (args & RAM_MASK)
        arg = cpu->RAM[arg];

    error = stackPush(cpu->call_stack, cpu->ip + sizeof(int));

    cpu->ip = arg;

})

DEF_CMD(ret, 19, 0, {
    cpu->ip++;
    int arg = 0;

    error = stackPop(cpu->call_stack, &arg);
    if (error)
        return error;
    cpu->ip = arg;
})
