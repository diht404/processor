call :set_ram
hlt
:set_ram
    push 20
    pop rax
    push 4
    pop rbx
    push 0
    pop rcx
    push 0
    pop rdx
    :2
            :1
                call :set_mem
                push rcx
                push 1
                add
                pop rcx
                push rcx
                push rax
            jb :1
            push 0
            pop rcx
        push rdx
        push 1
        add
        pop rdx
        push rdx
        push rax
    jb :2
    push 0
    pop rdx
    SHOW_RAM
    ret
:set_mem
    push rdx
    push rbx
    mod
    push 0
    jbe :set_zero
        push rcx
        push rbx
        mod
        push 0
    jbe :set_zero
        push rax
        push rcx
        mul
        push rdx
        add
        push 1
        SET_RAM
        ret
    :set_zero
        push rax
        push rcx
        mul
        push rdx
        add
        push 0
        SET_RAM
        ret