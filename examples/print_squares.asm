push 0
pop rax
:label
push rax
push rax
mul
out
push rax
push 1
add
pop rax
push rax
push 10
jb :label
PICTURE
hlt