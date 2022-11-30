;salto incondicional
jmp @inicio

;*******************************************
;contantes
;*******************************************
Raio equ 30

;*******************************************
;alocacao de variaveis
;*******************************************
a db -120
b db -10
c db 2
d dw 32394

;*******************************************
;operacoes aritmeticas
;*******************************************
@inicio:
mov al, 255
mov bl, 2
mul bl

mov ax, 65000
mov bl, 33
div bl

hlt

endp