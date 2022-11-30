;salto incondicional
jmp @inicio

@inicio

mov al, 10
mov ah, 5
@teste1:
sub al, 1
cmp al, ah
je @igual
jg @maior
jl @menor

@maior:
mov bx, 65535
jmp @teste1
@igual:
mov bx, 32768
jmp @teste1
@menor:
mov bx, 32768
cmp al, 0
jle @fimteste1
jmp @teste1
@fimteste1:


HLT