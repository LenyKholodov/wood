; Проверка присутствия MMX
; Copyright (C) 1997 by Sergey Zabaryansky
;
_DATA   SEGMENT public 'DATA'
        ASSUME  cs:_DATA, ds:_DATA
        ORG     100h

; Задаем команду CPUID
CPUID   macro
	DB	0Fh, 0A2h
	endm

; Тут начинается исполнение программы
start:  call    If_CPUID_Present
; Если мы работаем с CPU ниже P5, то MMX нет
        jz      Print_MMX_absent
.386
; Берем флаги возможностей процессора
	mov	eax, 1
	CPUID
; Тестируем 23 бит
	and	edx, 1 shl 23
	jz 	Print_MMX_absent
.8086
Print_MMX_Present:
; Выводим одноименную строку на экран
	lea	dx, msgPRESENT
	mov	ah, 9
	int	21h
	jmp	Quit

Print_MMX_absent:
; Тоже самое
        lea     dx, msgABSENT
        mov     ah, 9
        int     21h
Quit:
; Выходим в DOS
        mov     ax, 4C00h
        int     21h
.386
If_CPUID_Present proc near
        cli
        pushfd                ; EFLAGS - в стек
        pop     eax           ; а из стека в EAX
        mov     ebx, eax      ; запомним на будущее
        xor     eax, 200000h  ; установим 21 бит
        push    eax
        popfd                 ; и установим EFLAGS
        pushfd
        pop     eax           ; загрузим флаги в EAX
        push    ebx
        popfd                 ; восстановим EFLAGS
        sti
        xor     eax, ebx      ; а менялся ли бит ?
        ret
If_CPUID_Present endp

msgABSENT     DB   'MMX is ABSENT',0Dh,0Ah,'$'
msgPRESENT    DB   'MMX is PRESENT',0Dh,0Ah,'$'


_DATA   ENDS
        END     start
