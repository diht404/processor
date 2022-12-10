#include <stdlib.h>

//#define flags "-D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla "

#define flags ""
#define asm_file "../../lang"
//#define asm_file "print_squares"
//#define asm_file "quadratic"
//#define asm_file "factorial"
//#define asm_file "data"

int main()
{
    system("g++ " flags "assembler/main.cpp assembler/assembler.cpp common/utils.cpp stack/stack.cpp "
           "stack/stack_logs.cpp stack/stack_verification.cpp -o assembler/assembler");
    system("g++ " flags "disassembler/main.cpp disassembler/disassembler.cpp common/utils.cpp stack/stack.cpp "
           "stack/stack_logs.cpp stack/stack_verification.cpp -o disassembler/disassembler");
    system("g++ " flags "cpu/main.cpp cpu/cpu.cpp common/utils.cpp stack/stack.cpp "
           "stack/stack_logs.cpp stack/stack_verification.cpp -o cpu/cpu");
    system("./assembler/assembler examples/" asm_file".asm examples/" asm_file".code");
    system("./disassembler/disassembler examples/" asm_file".code examples/" asm_file".disasm");
    system("./cpu/cpu examples/" asm_file".code");
    system("rm ./examples/" asm_file".code");
    system("rm ./assembler/assembler ./disassembler/disassembler ./cpu/cpu ./main");
    return 0;
}