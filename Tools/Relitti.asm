
;======================RELITTI======================
macro WaitToRead {
@@:	in al, 0x64
	test al, 1
	jz @b
	}

macro WaitToWrite {
@@:	in al, 0x64
	test al, 10b
	jnz @b
	}
	
	
GDT:
	dd 0,0				;NULL
	dw 0xffff,0				;CODE
	db 0,10011000b,11001111b,0
	dw 0xffff,0				;DATA
	db 0,10010010b,11001111b,0
GDTend:
gdtr dw GDTend-GDT-1
	dd GDT
	
IDT:
	dw int_0 and 0x0000ffff			;int 0
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_1 and 0x0000ffff			;int 1
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_2 and 0x0000ffff			;int 2
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_3 and 0x0000ffff			;int 3
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_4 and 0x0000ffff			;int 4
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_5 and 0x0000ffff			;int 5
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_6 and 0x0000ffff			;int 6
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_7 and 0x0000ffff			;int 7
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_8 and 0x0000ffff			;int 8
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_9 and 0x0000ffff			;int 9
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_10 and 0x0000ffff		;int 10
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_11 and 0x0000ffff		;int 11
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_12 and 0x0000ffff		;int 12
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_13 and 0x0000ffff		;int 13
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_14 and 0x0000ffff		;int 14
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_15 and 0x0000ffff		;int 15
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_16 and 0x0000ffff		;int 16
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_17 and 0x0000ffff		;int 17
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_18 and 0x0000ffff		;int 18
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_19 and 0x0000ffff		;int 19
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_20 and 0x0000ffff		;int 20
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_21 and 0x0000ffff		;int 21
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_22 and 0x0000ffff		;int 22
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_23 and 0x0000ffff		;int 23
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_24 and 0x0000ffff		;int 24
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_25 and 0x0000ffff		;int 25
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_26 and 0x0000ffff		;int 26
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_27 and 0x0000ffff		;int 27
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_28 and 0x0000ffff		;int 28
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_29 and 0x0000ffff		;int 29
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_30 and 0x0000ffff		;int 30
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_31 and 0x0000ffff		;int 31
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	;==========================INIZIANO=QUELLE=DEL=PIC=========================
	dw int_32 and 0x0000ffff		;int 32
	dw 0x08
	db 0,0x8e
	dw segm_2sect
	
	dw int_33 and 0x0000ffff		;int 33
	dw 0x08
	db 0,0x8e
	dw segm_2sect
IDTend:
idtr dw IDTend-IDT-1
	dd IDT
	
;=========================RICHIESTA RAM=========================
	rammap=0x500
	xor si, si
	mov ax, rammap shr 4	;Dopo la BDA, primo spazio libero - nota il segmento
	mov es, ax
	xor di, di
	xor ebx, ebx
	mov edx, 0x534D4150
	mov eax, 0xE820
	mov ecx, 24
	int 0x15
	jc STOP
	cmp eax, 0x534D4150
	jne STOP
	inc si
	
@@:	add di, 24
	mov eax, 0xE820
	mov ecx, 24
	int 0x15
	inc si
	jc ESCI
	test ebx, ebx
	jz ESCI
	jmp @b
ESCI:	;add si, 1
	mov word [RAMEntries], si	;Memorizza il numero di entries
	
;=========================MANAGEMENT RAM========================
	mov ebx, 0xB8000+0*160
	mov esi, rammap
	movzx edi, word [RAMEntries]
	lea ebp, [esi+edi]	;Subito dopo la regione gia occupata
@@:	mov eax, dword [esi+4]
	call printoxd
	add ebx, 16
	mov eax, dword [esi]
	call printoxd
	add ebx, 16
	add ebx, 2
	mov eax, dword [esi+12]
	call printoxd
	add ebx, 16
	mov eax, dword [esi+8]
	call printoxd
	add ebx, 16
	add ebx, 2
	mov eax, dword [esi+20]
	call printoxd
	add ebx, 16
	mov eax, dword [esi+16]
	call printoxd
	
	cmp eax, 1
	jne .C3
	mov eax, dword [esi]
	mov dword [ebp], eax
	mov eax, dword [esi+4]
	mov dword [ebp+4], eax
	mov eax, dword [esi+8]
	mov dword [ebp+8], eax
	mov eax, dword [esi+12]
	mov dword [ebp+12], eax
	
.C3:	add ebx, 16
	add ebx, 160-100
	add esi, 24
	add ebp, 16
	dec edi
	jnz @b