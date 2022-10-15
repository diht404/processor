call :in
call :push_regs
call :func
hlt
:func
    push rax
    push 0
    je :a_zero
    push rax
    push 0
    jne :a_not_zero
    ret
:in
    push 1
    push 4
    push 2
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
    push rcx
    push 0
    je :abc_zero
    push rcx
    push 0
    jne :ab_zero_c_not
    ret
:abc_zero
    any_num
    ret
:ab_zero_c_not
    no_sols
    ret
:b_not_zero
    push rcx
    push -1
    mul
    push rbx
    div
    out
    ret
:a_not_zero
    push rbx
    push 0
    je :a_not_zero_b_zero
    push rbx
    push rbx
    mul
    push 4
    push rax
    mul
    push rcx
    mul
    sub
    pop rdx
    push rdx
    push 0
    jb :d_below_zero
    push rdx
    push 0
    je :d_zero
    push rdx
    push 0
    ja :d_above_zero
    ret
:a_not_zero_b_zero
    push rcx
    push 0
    je :bc_zero
    push rcx
    push 0
    jne :b_zero_c_not_zero
    ret
:bc_zero
    push 0
    out
    ret
:b_zero_c_not_zero
    push rcx
    push rax
    div
    sqrt
    out
    push rcx
    push rax
    div
    sqrt
    push -1
    mul
    out
    ret
:d_below_zero
    no_sols
    ret
:d_above_zero
    push rbx
    push -1
    mul
    push rdx
    sqrt
    add
    push 2
    push rax
    mul
    div
    out
    push rbx
    push -1
    mul
    push rdx
    sqrt
    sub
    push 2
    push rax
    mul
    div
    out
    ret
:d_zero
    push rbx
    push -1
    mul
    push rdx
    sqrt
    add
    push 2
    push rax
    mul
    div
    out
    ret