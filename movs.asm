;*******************************************
;instrucoes de movimentacao de dados
;*******************************************
jmp @inicio

Raio equ 30

a db -10
b db 1
c dw 2
d dw 32394

@inicio:

;imediato para registrador
mov bh, 50
mov al, 10
mov ah, 50
mov bl, 20
mov bh, 50
mov ax, 1000
mov bx, 2000
mov ax, Raio
mov bl, Raio

;registrador para registrador
mov al, bl
mov ax, bx

;memoria para registrador
mov al, a
mov bl, b
mov ax, c
mov bx, d

;registrador para memoria
mov al, 5
mov b, al
mov ah, b
mov ax, 2
mov d, ax
mov ax, d


hlt

