;salto incondicional
jmp @inicio

;*******************************************
;contantes
;*******************************************
Raio equ 30

;*******************************************
;alocacao de variaveis
;*******************************************
a db -10
b db 1
c dw 2
d dw 32394

;*******************************************
;operacoes logicas
;*******************************************
@inicio:
mov ah, 10
mov al, 5
and ah, al
and ah, 4
and al, b
mov ax, 100
and ax, 100
mov ah, 10
mov al, 5
or al, ah
or ah, 4
or al, b
mov ah, 10
mov al, 5
xor al, ah
xor ah, 4
xor al, b
xor bx, bx
not ax

hlt