; �������� ����������� MMX
; Copyright (C) 1997 by Sergey Zabaryansky
;
_DATA   SEGMENT public 'DATA'
        ASSUME  cs:_DATA, ds:_DATA
        ORG     100h

; ������ ������� CPUID
CPUID   macro
	DB	0Fh, 0A2h
	endm

; ��� ���������� ���������� ���������
start:  call    If_CPUID_Present
; ���� �� �������� � CPU ���� P5, �� MMX ���
        jz      Print_MMX_absent
.386
; ����� ����� ������������ ����������
	mov	eax, 1
	CPUID
; ��������� 23 ���
	and	edx, 1 shl 23
	jz 	Print_MMX_absent
.8086
Print_MMX_Present:
; ������� ����������� ������ �� �����
	lea	dx, msgPRESENT
	mov	ah, 9
	int	21h
	jmp	Quit

Print_MMX_absent:
; ���� �����
        lea     dx, msgABSENT
        mov     ah, 9
        int     21h
Quit:
; ������� � DOS
        mov     ax, 4C00h
        int     21h
.386
If_CPUID_Present proc near
        cli
        pushfd                ; EFLAGS - � ����
        pop     eax           ; � �� ����� � EAX
        mov     ebx, eax      ; �������� �� �������
        xor     eax, 200000h  ; ��������� 21 ���
        push    eax
        popfd                 ; � ��������� EFLAGS
        pushfd
        pop     eax           ; �������� ����� � EAX
        push    ebx
        popfd                 ; ����������� EFLAGS
        sti
        xor     eax, ebx      ; � ������� �� ��� ?
        ret
If_CPUID_Present endp

msgABSENT     DB   'MMX is ABSENT',0Dh,0Ah,'$'
msgPRESENT    DB   'MMX is PRESENT',0Dh,0Ah,'$'


_DATA   ENDS
        END     start
