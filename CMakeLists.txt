cmake_minimum_required(VERSION 3.23)
project(stack C CXX)

set(CMAKE_C_STANDARD 99)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D _DEBUG -ggdb3 -std=c++17 -O0 -Wall -Wextra -Weffc++ -Waggressive-loop-optimizations -Wc++14-compat -Wmissing-declarations -Wcast-align -Wcast-qual -Wchar-subscripts -Wconditionally-supported -Wconversion -Wctor-dtor-privacy -Wempty-body -Wfloat-equal -Wformat-nonliteral -Wformat-security -Wformat-signedness -Wformat=2 -Winline -Wlogical-op -Wnon-virtual-dtor -Wopenmp-simd -Woverloaded-virtual -Wpacked -Wpointer-arith -Winit-self -Wredundant-decls -Wshadow -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=2 -Wsuggest-attribute=noreturn -Wsuggest-final-methods -Wsuggest-final-types -Wsuggest-override -Wswitch-default -Wswitch-enum -Wsync-nand -Wundef -Wunreachable-code -Wunused -Wuseless-cast -Wvariadic-macros -Wno-literal-suffix -Wno-missing-field-initializers -Wno-narrowing -Wno-old-style-cast -Wno-varargs -Wstack-protector -fcheck-new -fsized-deallocation -fstack-protector -fstrict-overflow -flto-odr-type-merging -fno-omit-frame-pointer -Wlarger-than=8192 -Wstack-usage=8192 -pie -fPIE -Werror=vla ")

add_executable(stack stack/main.cpp stack/stack.cpp stack/stack_logs.cpp stack/stack_verification.cpp)

add_executable(main main.cpp)
add_executable(assembler_run assembler/main.cpp assembler/assembler.cpp utils.cpp
        stack/stack.cpp stack/stack_logs.cpp stack/stack_verification.cpp)
add_executable(disassembler_run disassembler/main.cpp disassembler/disassembler.cpp utils.cpp
        stack/stack.cpp stack/stack_logs.cpp stack/stack_verification.cpp)
add_executable(cpu_run cpu/main.cpp cpu/cpu.cpp utils.cpp
        stack/stack.cpp stack/stack_logs.cpp stack/stack_verification.cpp)