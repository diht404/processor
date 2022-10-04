//#include "stdio.h"
#include <stdlib.h>

#define flags "-Wshadow -Winit-self -Wredundant-decls -Wcast-align -Wundef -Wfloat-equal -Winline -Wunreachable-code -Wmissing-declarations -Wmissing-include-dirs -Wswitch-enum -Wswitch-default -Weffc++ -Wmain -Wextra -Wall -g -pipe -fexceptions -Wcast-qual -Wconversion -Wctor-dtor-privacy -Wempty-body -Wformat-security -Wformat=2 -Wignored-qualifiers -Wlogical-op -Wno-missing-field-initializers -Wnon-virtual-dtor -Woverloaded-virtual -Wpointer-arith -Wsign-promo -Wstack-usage=8192 -Wstrict-aliasing -Wstrict-null-sentinel -Wtype-limits -Wwrite-strings -D_DEBUG -D_EJUDGE_CLIENT_SIDE "

#define flags ""
int main()
{
    system("g++ " flags "assembler.cpp utils.cpp stack/stack.cpp "
           "stack/stack_logs.cpp stack/stack_verification.cpp -o asm");
    system("g++ " flags "cpu.cpp utils.cpp stack/stack.cpp "
           "stack/stack_logs.cpp stack/stack_verification.cpp -o cpu");
    system("asm");
    system("cpu");
    system("rm data.code");
    system("rm *.exe");
    return 0;
}
