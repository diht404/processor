in
call :factorial
out
hlt
:factorial
    call :factorial_rax
    push rax
    ret
:factorial_rax
    push rax
    push 1
    jae :1
        push 1
        pop rax
    :1
    pop rbx
    push rbx
    push rax
    mul
    pop rax
    push rbx
    push 1
    sub
    pop rbx
    push rbx
    push 1
    jae :2
        ret
    :2
    push rbx
    call :factorial_rax
    ret