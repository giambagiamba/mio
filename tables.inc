align 4
	MEMMAP dw 0,0,0	;Mappa memoria bios - dd addr : dw entries
	MEM_BITMAP dd 0xffffffff,0,0,0	;qtà memoria disponibile - indirizzo bitmap 4k - indirizzo bitmap 64k - indirizzo bitmap 1M
	TASKLIST dd 0,0
	INTHAND dd INT32_Default,INT33_Default,INT40_Default,INT41_Default
	num_int db '000',0
	GDT_1 dw 0xffff,0
	GDT_2C db 0,10011000b,11001111b,0
	GDT_2D db 0,10010010b,11001111b,0
	IDT_1 db 0x08,0,0,0x8E
	gdtr dw 0,0,0
	idtr dw 0,0,0
	EnterUser db 'User:',0
	CurrentUser db 'Giamba',0,0,0,0,0,0,0,0,0,0,0,0,0,0
	CommandLineBeg db 'MioOS@',0
	CommandLineEnd db '>',0
	CurrentAddress db '|A:\',0

addr_GDT=0x0500
addr_IDT=0x0520
BuildGDT:
	xor eax, eax
	mov dword [addr_GDT], eax		;NULL selector
	mov dword [addr_GDT+4], eax
	mov ebx, dword [GDT_1]			;CODE selector
	mov eax, dword [GDT_2C]
	mov dword [addr_GDT+8], ebx
	mov dword [addr_GDT+12], eax
	mov eax, dword [GDT_2D]			;DATA selector
	mov dword [addr_GDT+16], ebx
	mov dword [addr_GDT+20], eax
	mov word [gdtr], 23				;Lunghezza
	mov word [gdtr+2], addr_GDT		;Indirizzo
	ret


BuildIDT:
	xor ebx, ebx
	mov eax, dword [IDT_1]
	mov dx, (int_0 and 0x0000ffff)
@@:	mov word [addr_IDT+ebx], dx
	mov dword [addr_IDT+ebx+2], eax
	mov word [addr_IDT+ebx+6], segm_2sect
	add ebx, 8
	add dx, (int_1-int_0)
	cmp ebx, 8*32
	jb @b
	mov word [addr_IDT+ebx], (int_32 and 0x0000ffff)
	mov dword [addr_IDT+ebx+2], eax
	mov word [addr_IDT+ebx+6], segm_2sect
	add ebx, 8
	mov word [addr_IDT+ebx], (int_33 and 0x0000ffff)
	mov dword [addr_IDT+ebx+2], eax
	mov word [addr_IDT+ebx+6], segm_2sect
	add ebx, 8
	mov dx, (int_34 and 0x0000ffff)
@@:	mov word [addr_IDT+ebx], dx
	mov dword [addr_IDT+ebx+2], eax
	mov word [addr_IDT+ebx+6], segm_2sect
	add ebx, 8
	add dx, (int_35-int_34)
	cmp ebx, 8*40
	jb @b
	mov word [addr_IDT+ebx], (int_40 and 0x0000ffff)
	mov dword [addr_IDT+ebx+2], eax
	mov word [addr_IDT+ebx+6], segm_2sect
	
	mov word [idtr], (8*41)-1
	mov word [idtr+2], addr_IDT
	ret
	

	use32
	
int_0:
	mov dword [num_int], '0  '
	call PrintInt
	iret
int_1:
	mov dword [num_int], '1  '
	call PrintInt
	iret
int_2:
	mov dword [num_int], '2  '
	call PrintInt
	iret
int_3:
	mov dword [num_int], '3  '
	call PrintInt
	iret
int_4:
	mov dword [num_int], '4  '
	call PrintInt
	iret
int_5:
	mov dword [num_int], '5  '
	call PrintInt
	iret
int_6:
	mov dword [num_int], '6  '
	call PrintInt
	iret
int_7:
	mov dword [num_int], '7  '
	call PrintInt
	iret
int_8:
	mov dword [num_int], '8  '
	call PrintInt
	iret
int_9:
	mov dword [num_int], '9  '
	call PrintInt
	iret
int_10:
	mov dword [num_int], '10 '
	call PrintInt
	iret
int_11:
	mov dword [num_int], '11 '
	call PrintInt
	iret
int_12:
	mov dword [num_int], '12 '
	call PrintInt
	iret
int_13:
	mov dword [num_int], '13 '
	call PrintInt
	iret
int_14:
	mov dword [num_int], '14 '
	call PrintInt
	iret
int_15:
	mov dword [num_int], '15 '
	call PrintInt
	iret
int_16:
	mov dword [num_int], '16 '
	call PrintInt
	iret
int_17:
	mov dword [num_int], '17 '
	call PrintInt
	iret
int_18:
	mov dword [num_int], '18 '
	call PrintInt
	iret
int_19:
	mov dword [num_int], '19 '
	call PrintInt
	iret
int_20:
	mov dword [num_int], '20 '
	call PrintInt
	iret
int_21:
	mov dword [num_int], '21 '
	call PrintInt
	iret
int_22:
	mov dword [num_int], '22 '
	call PrintInt
	iret
int_23:
	mov dword [num_int], '23 '
	call PrintInt
	iret
int_24:
	mov dword [num_int], '24 '
	call PrintInt
	iret
int_25:
	mov dword [num_int], '25 '
	call PrintInt
	iret
int_26:
	mov dword [num_int], '26 '
	call PrintInt
	iret
int_27:
	mov dword [num_int], '27 '
	call PrintInt
	iret
int_28:
	mov dword [num_int], '28 '
	call PrintInt
	iret
int_29:
	mov dword [num_int], '29 '
	call PrintInt
	iret
int_30:
	mov dword [num_int], '30 '
	call PrintInt
	iret
int_31:
	mov dword [num_int], '31 '
	call PrintInt
	iret
	;==========================INIZIANO=QUELLE=DEL=PIC=========================
int_32:
	mov dword [num_int], '32 '
	call PrintInt
	call [INTHAND]
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_33:
	mov dword [num_int], '33 '
	call PrintInt
	call [INTHAND+4]
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_34:
	mov dword [num_int], '34 '
	call PrintInt
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_35:
	mov dword [num_int], '35 '
	call PrintInt
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_36:
	mov dword [num_int], '36 '
	call PrintInt
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_37:
	mov dword [num_int], '37 '
	call PrintInt
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_38:
	mov dword [num_int], '38 '
	call PrintInt
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_39:
	mov dword [num_int], '39 '
	call PrintInt
	push ax
	mov al, 0x20	;EOI
	out 0x20, al
	pop ax
	iret
int_40:
	mov dword [num_int], '40 '
	call PrintInt
	call [INTHAND+8]
	push ax
	mov al, 0x8C	;Lettura fuffa registro C
	out 0x70, al
	in al, 0x71
	mov al, 0x20	;EOI
	out 0xA0, al
	mov al, 0x20
	out 0x20, al
	pop ax
	iret
int_41:
	mov dword [num_int], '40 '
	call PrintInt
	call [INTHAND+12]
	iret
	
	


PrintInt:
	push ebx
	push edx
	push cx
	mov edx, num_int
	mov ebx, 0xb8000+(24*160)+8
	mov ch, 2
.P:	mov cl, byte [edx]
	test cl, cl
	jz @f
	mov word [ebx], cx
	add edx, 1
	add ebx, 2
	jmp .P
@@:	
	; call print
	pop cx
	pop edx
	pop ebx
	ret

