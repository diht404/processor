call :in
call :push_regs
call :func
hlt
:func
    push rax
    push 0
    je :a_zero
    ret
:in
    push 0
    push 2
    push 6
    ret
:push_regs
    pop rcx
    pop rbx
    pop rax
    ret
:a_zero
    push rbx
    push 0
    je :ab_zero
    push rbx
    push 0
    jne :b_not_zero
    ret
:ab_zero
    push rbx
    push 0
    je :abc_zero
    push rbx
    push 0
    jne :ab_zero_c_not
    ret
:abc_zero
    push 666
    out
    ret
:ab_zero_c_not
    push -666
    out
    ret
:b_not_zero
    push rcx
    push -1
    mul
    push rbx
    div
    out
    ret