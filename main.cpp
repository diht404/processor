#include "stack/stack.h"
#include "stack/stack_logs.h"

int main()
{
    Stack stack = {};
    setLogFile("logs.txt");
    atexit(closeLogFile);

    size_t error = STACK_NO_ERRORS;

    stackCtor(&stack, 0, &error)

    for (int i = 0; i < 1024; i++)
    {
        error = stackPush(&stack, i);
    }
    for (int i = 0; i < 1024; i++)
    {
        Elem_t value = 0;
        error = stackPop(&stack, &value);
    }
    stackShrinkToFit(&stack);
    error = stackDtor(&stack);

    Elem_t value = POISON_INT_VALUE;
    error = stackPop(&stack, &value);

    error = stackPush(&stack, 666);
    return 0;
}
