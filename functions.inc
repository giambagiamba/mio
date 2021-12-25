
include 'decoders.inc'

macro Wait {
	out 0x80, al
}

WaitToRead:
@@:	in al, 0x64
	test al, 1
	jz @b
	ret

WaitToWrite:
@@:	in al, 0x64
	test al, 10b
	jnz @b
	ret

IDLE:
	hlt
	jmp IDLE


EnableGate:		;Abilita Gate - Si è rivelato fungente
	in al, 0x92
	or al, 2
	out 0x92, al
	ret


TestGate:	;Assume: ebx->Puntatore video - ch->ColorAttributes
	;Ritorna: 0 se gate disabilitato, 1 se abilitato
	mov eax, 0x112345
	mov ebp, 0x012345
	mov dword [eax], eax
	mov dword [ebp], ebp
	cmp ebp, dword [eax]	;Se eguali->gate disabled
	je @f
	mov al, 1
	ret
@@:
	xor al, al
	ret


Init8259A:
	mov al, (0x10 or 0x01)	;Init command
	out 0x20, al
	Wait
	out 0xA0, al
	Wait
	
	mov al, 0x20	;Offset master
	out 0x21, al
	Wait
	mov al, 0x28	;Offset slave
	out 0xA1, al
	Wait
		
	mov al, 100b	;Master ha lo slave collegato alla porta 2
	out 0x21, al
	Wait
	mov al, 010b	;Porta di collegamento dello slave
	out 0xA1, al
	Wait
	
	mov al, 1		;8086
	out 0x21, al
	Wait
	out 0xA1, al
	Wait
	
	xor al, al		;Reset data ports
	out 0x21, al
	Wait
	out 0xA1, al
	Wait
	ret
	
	
Init8242_KBD:
	; cli
	mov al, 0xA7	;Disable porte
	out 0x64, al
	call WaitToWrite
	mov al, 0xAD
	out 0x64, al
	call WaitToWrite
	
@@:	in al, 0x60		;Siamo sicuri di averlo svuotato
	Wait
	in al, 0x64
	test al, 1
	jnz @b
	
	call WaitToWrite
	mov al, 0x20	;Richiesta ReadControllerConfigurationByte
	out 0x64, al
	call WaitToRead
	in al, 0x60
	and al, 10111101b	;Inibizione translation & IRQ seconda porta
	mov cl, al
	call WaitToWrite
	mov al, 0x60	;Richiesta WriteControllerConfigurationByte
	out 0x64, al
	call WaitToWrite
	mov al, cl
	out 0x60, al
	
	call WaitToWrite		;Ports test
	mov al, 0xAB	;Prima porta
	out 0x64, al
	call WaitToRead
	in al, 0x60
	test al, al
	jz @f
		mov ebx, (0xB8000+24*160+100)
		mov edx, str_port
		call print
		JMP IDLE
	
@@:	call WaitToWrite		;Enable port
	mov al, 0xAE	;Solo la prima
	out 0x64, al
	
	call WaitToWrite
	mov al, 0xF0	;ScanCode function
	out 0x60, al
	call WaitToRead
	in al, 0x60
	call WaitToWrite
	mov al, 2		;SetScanCode
	out 0x60, al
	call WaitToRead
	in al, 0x60
	; sti
	ret


	
InitRTC:
	; cli
	push ax
	mov al, 0x8B
	out 0x70, al
	Wait
	in al, 0x71
	mov ah, al
	mov al, 0x8B
	out 0x70, al
	mov al, ah
	or al, 0x40
	Wait
	out 0x71, al
	mov al, 0x8A
	out 0x70, al
	Wait
	in al, 0x71
	mov ah, al
	and ah, 0xF0
	or ah, 15
	mov al, 0x8A
	out 0x70, al
	Wait
	mov al, ah
	out 0x71, al
	pop ax
	; sti
	ret
	
	
	
DrawGUI:
	mov cl, ASCII_H
	mov ebx, 0xB8000+22*160
@@:	mov word [ebx], cx
	add ebx, 2
	cmp ebx, 0xB8000+23*160
	jb @b
	
	mov cl, ASCII_Tgg
	mov word [0xB8000+22*160+38], cx
	mov cl, ASCII_V
	mov word [0xB8000+23*160+38], cx
	mov word [0xB8000+24*160+16], cx
	mov word [0xB8000+24*160+82], cx
	
	mov edx, str_cnt
	mov ebx, 0xB8000+23*160
	call print
	mov edx, str_KBD
	mov ebx, 0xB8000+24*160+20
	call print
	mov edx, str_INT
	mov ebx, 0xb8000+24*160
	call print
	ret
	

Malloc:			;distrugge eax, ma è ovvio
	; xor eax, eax
	sub esp, 12
	mov dword [esp], edi
	mov dword [esp+4], edx
	mov dword [esp+8], ecx
	mov edi, dword [esp+12]		;argomento in bytes
	shr edi, 12				;argomento in pagine
	mov edx, dword [MEM_BITMAP]	;qtà memoria disponibile
	cmp edi, edx
	ja .E1
	mov edx, dword [MEM_BITMAP+4]	;indirizzo bitmap
.L1:	mov ax, 32
	mov ecx, dword [edx]
@@:	test ecx, 1
	jz .A1		;cerca altra regione
	sub edi, 1	;scala conteggio di una pagina
	jz .F1		;fine
	shr ecx, 1
	sub ax, 1
	ja @b		;analizza tutto ecx
	add edx, 4	;una nuova dword
	jmp .L1
	
.A1:	shr ecx, 1
	test ecx, 1
	
	
.F1:	
.E1:	xor eax, eax
	mov edi, dword [esp]
	mov edx, dword [esp+4]
	mov ecx, dword [esp+8]
	add esp, 12
	ret
	
	
DrawWindow:
;ebx:Posizione partenza relativa - word[esp]:Larghezza - word[esp+2]:Altezza - ch:Sfondo
	add ebx, 0xB8000
	movzx eax, word [esp+4]	;Ricordare che c'è stata call
	mov edx, eax
	mov cl, ASCII_Aas
	mov word [ebx], cx
	mov cl, ASCII_H
@@:	add ebx, 2
	mov word [ebx], cx
	sub eax, 1
	jnz @b
	add ebx, 2
	mov cl, ASCII_Aad
	mov word [ebx], cx
	
	mov bp, word [esp+6]
	mov eax, 80
	sub eax, edx
.A:	mov cl, ASCII_V
	lea ebx, [ebx+2*eax-2]
	mov word [ebx], cx
	mov esi, edx
@@:	add ebx, 2
	xor cl, cl
	mov word [ebx], cx
	sub esi, 1
	jnz @b
	mov cl, ASCII_V
	add ebx, 2
	mov word [ebx], cx
	sub bp, 1
	jnz .A
	
	lea ebx, [ebx+2*eax-2]
	mov cl, ASCII_Abs
	mov word [ebx], cx
	mov cl, ASCII_H
@@:	add ebx, 2
	mov word [ebx], cx
	sub edx, 1
	jnz @b
	add ebx, 2
	mov cl, ASCII_Abd
	mov word [ebx], cx
	ret
	


;========================ALCUNE VARIABILI===========================
align 4
	; s dd 0xdabb2a55,0x15ff2000,0xda5b2b55,0x300f2040
	; time dd 0,0
	; RAMEntries dw 0
	KBD_INT_COUNT dw 0
	str_gate db '>>GATE ENABLE FAIL!!<<',0
	str_port db '>>FIRST PORT CHECK FAILED!!<<',0
	str_KBD db 'KBD:',0
	str_INT db 'INT ',0
	str_cnt db 'KBD INT COUNT ',0
	IsCaretActive db 0
	ASCII_H=205
	ASCII_V=186
	ASCII_Tgg=203
	ASCII_Tsu=202
	ASCII_Tsx=185
	ASCII_Tdx=204
	ASCII_Aas=201
	ASCII_Aad=187
	ASCII_Abs=200
	ASCII_Abd=188


	
;=======================HANDLERS INDIRETTI==========================
NULL:
	ret
	
INT32_Default:
	jmp SWITCH
	
INT40_Default:
	rol byte [ebx+1], 4
	xor byte [IsCaretActive], 1
	; jmp SWITCH
	ret
	
INT41_Default:
	jmp SWITCH
	
SWITCH:
	sub esp, 8			;Salvataggio vecchi registri
	mov dword [esp], edx
	mov dword [esp+4], esi
	mov esi, dword [TASKLIST]
	movzx edx, word [TASKLIST+4]
	mov esi, dword [esi+8*edx]
	mov dword [esi+4], eax
	mov eax, dword [esp+12]	;eip
	mov dword [esi], eax	;
	mov eax, dword [esp+20]	;eflags
	mov dword [esi+36], eax	;
	mov dword [esi+8], ebx
	mov dword [esi+12], ecx
	mov edx, dword [esp]	;edx
	mov dword [esi+16], edx	;
	mov edx, dword [esp+4]	;esi
	mov dword [esi+20], edx	;
	mov dword [esi+24], edi
	lea edx, [esp+24]		;esp
	mov dword [esi+28], edx	;
	mov dword [esi+32], ebp
	
	mov esi, dword [TASKLIST]	;Ricerca prossimo processo
	mov bx, word [TASKLIST+4]
	mov di, word [TASKLIST+6]
@@:	add bx, 1
	mov ax, bx
	xor dx, dx
	div di
	movzx edx, dx
	test byte [esi+8*edx+4], 1
	jnz @b
	
	mov word [TASKLIST+4], dx
	mov esi, dword [esi+8*edx]
	
	mov ebp, esp			;Inserimento registri nuovo processo
	mov esp, dword [esi+28]
	sub esp, 16			;esp
	mov eax, dword [ebp+8]	;prima ret
	mov dword [esp], eax	;
	mov eax, dword [esi]	;eip
	mov dword [esp+4], eax	;
	mov eax, dword [ebp+16]	;cs
	mov dword [esp+8], eax	;
	mov eax, dword [esi+36]	;eflags
	mov dword [esp+12], eax	;
	mov eax, dword [esi+4]
	mov ebx, dword [esi+8]
	mov ecx, dword [esi+12]
	mov edx, dword [esi+16]
	mov edi, dword [esi+24]
	mov ebp, dword [esi+32]
	mov esi, dword [esi+20]
	
	ret	;Se eip è impiantato bene dovrebbe andare da se al task giusto
	
	
INT33_Default:
	push eax
	push edi
	push edx
	push ebx
	PUSH CX
	
	mov ch, 2
	inc word [KBD_INT_COUNT]
	mov ebx, 0xB8000+23*160+28
	mov ax, word [KBD_INT_COUNT]
	call printoxw
	
	mov ebx, 0xB8000+24*160+30
	mov edx, decoded
	call WaitToRead
	in al, 0x60
	call printoxb
	cmp al, 0xF0	;SingleByte Break
	je .SBB
	cmp al, 0xE0	;DualByte (Make or Break)
	je .DB
	call d1		;Se arriva qui è un SingleByte Make
	jmp .F

.SBB:
	add ebx, 6		;SingleByte Break
	call WaitToRead
	in al, 0x60
	call printoxb
	call d2
	jmp .F
	
.DB:	add ebx, 6		;DualByte
	call WaitToRead
	in al, 0x60
	call printoxb
	cmp al, 0xF0	;DualByte Break
	je .DBB
	call d3
	jmp .F

.DBB:
	add ebx, 6		;DualByte Break
	call WaitToRead
	in al, 0x60
	call printoxb
	call d4
	JMP .F
.F:
@@:	cmp ebx, (0xB8000+24*160+78)	;Cancella i rimasugli sulla riga
	jae @f
	add ebx, 6
	mov dword [ebx], 0
	jmp @b
	
@@:	POP CX
	pop ebx
	call print
	mov byte [decoded], 0
	
	cmp ebx, 0xB8000
	ja @f
	test byte [IsCaretActive], 1
	jz .E1
	rol byte [ebx+1], 4
	rol byte [0xB8000+1], 4
.E1:	mov ebx, 0xB8000
	jmp .E
@@:	cmp ebx, 0xB8000+160*22
	jb .E
	test byte [IsCaretActive], 1
	jz .E2
	rol byte [ebx+1], 4
	rol byte [ebx-160*22+1], 4
.E2:	sub ebx, 160*22
.E:	pop edx
	pop edi
	pop eax
	ret
	
	
;========================FUNZIONI DI PRINT==========================
print:	;Assume: edx->Puntatore striga - ebx->Puntatore video - ch->ColorAttributes
	test byte [IsCaretActive], 1
	jz .P
	rol byte [ebx+1], 4
.P:	mov cl, byte [edx]
	test cl, cl
	jz @f
	mov word [ebx], cx
	add edx, 1
	add ebx, 2
	jmp .P
@@:	
	test byte [IsCaretActive], 1
	jz .F
	rol byte [ebx+1], 4
.F:	ret


printoxb:	;Assume: al->byte da stampare - ch - ebx
	;Funzione non distruttiva
	push edx
	push cx
	
	ror eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+2], cx
	
	pop cx
	pop edx
	ret


printoxw:	;Assume: ax->word da stampare - ch - ebx
	;Funzione non distruttiva
	push edx
	push cx
	
	ror eax, 12
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+2], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+4], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+6], cx
	
	pop cx
	pop edx
	ret


printoxd:	;Assume: eax->dword da stampare - ch - ebx
	;Funzione non distruttiva
	push edx
	push cx
	
	ror eax, 28
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+2], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+4], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+6], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+8], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+10], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+12], cx
	rol eax, 4
	mov edx, eax
	and edx, 0x0000000f
	mov cl, byte [ox+edx]
	mov word [ebx+14], cx
	
	pop cx
	pop edx
	ret
ox db '0123456789ABCDEF'
