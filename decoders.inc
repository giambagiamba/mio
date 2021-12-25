
	;Assumono codice in ax - decoded (pointer) in edx
	
;==========================================================
d1:	mov edi, dword [KS]	;Decoder for SingleByte Makes
	cmp al, 0x14
	ja X1		;Evita i check per i primi tasti poco usati
	cmp al, 0x0C
	ja @f
	ret
@@:	cmp al, 0x0D	;TAB
	ja @f
	mov edi, dword [ESP+6]
	test byte [IsCaretActive], 1
	jz .NT
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi+9], 4		;Nuovo caret
.NT:	add dword [ESP+6], 8
	ret
@@:	cmp al, 0x0E	;Il primo carattere spaiato
	ja @f
	mov al, byte [edi+67]
	mov byte [edx], al
	ret
@@:	cmp al, 0x11	;LAlt
	ja @f
	ret
@@:	cmp al, 0x12	;LShift
	ja @f
	cmp dword [KS], KS2	;Già shiftato
	je C2
	cmp dword [KS], KS4	;Già shiftato
	je C2
	add dword [KS], 71
C2:	ret
@@:	cmp al, 0x14	;LCtrl
	ja @f
	ret
X1:
@@:	cmp al, 0x57	;Gruppo principale di caratteri
	ja @f
	movzx eax, al
	mov al, byte [edi+eax-0x15]
	mov byte [edx], al
	ret
@@:	cmp al, 0x58	;CAPS
	ja @f
	cmp dword [KS], KS2
	jbe C1
	sub dword [KS], 142
	ret
C1:	add dword [KS], 142
	ret
@@:	cmp al, 0x59	;RShift
	ja @f
	cmp dword [KS], KS2	;Già shiftato
	je C2
	cmp dword [KS], KS4	;Già shiftato
	je C2
	add dword [KS], 71
	ret
@@:	cmp al, 0x5A	;Enter
	ja @f
	push eax
	push edx
	push ebx
	mov edi, dword [ESP+18]
	
	xor edx, edx
	mov eax, edi
	sub eax, 0xB8000
	mov ebx, 160
	div ebx		;Modulo in edx
	neg edx
	add edx, 160
	test byte [IsCaretActive], 1
	jz .NE
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi+edx+1], 4		;Nuovo caret
.NE:	add dword [ESP+18], edx
	
	pop ebx
	pop edx
	pop eax
	
	ret
@@:	cmp al, 0x5D		;Gli ultimi due caratteri
	ja @f
	movzx eax, al
	mov al, byte [edi+68+eax-0x5B]
	mov byte [edx], al
	ret
@@:	cmp al, 0x66		;Backspeis
	ja @f
	mov edi, dword [ESP+6]
	test byte [IsCaretActive], 1
	jz .NB
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi-1], 4		;Nuovo caret
.NB:	sub edi, 2
	mov byte [edi], ' '
	mov dword [ESP+6], edi
	ret
@@:	cmp al, 0x7D		;La roba del keypad
	ja @f
	movzx eax, al
	mov al, byte [KP1+eax-0x69]
	mov byte [edx], al
	ret
@@:	ret

;==========================================================
d2:	cmp al, 0x11		;Decoder for breaks
	ja @f
	ret
@@:	cmp al, 0x12	;LShift
	ja @f
	sub dword [KS], 71
	ret
@@:	cmp al, 0x58
	ja @f
	ret
@@:	cmp al, 0x59	;RShift
	ja @f
	sub dword [KS], 71
	ret
@@:	ret

;==========================================================
d3:	cmp al, 0x48
	ja @f
	ret
@@:	cmp al, 0x4A
	ja @f
	mov byte [edx], '/'
	ret
@@:	cmp al, 0x5E
	ja @f
	ret
@@:	cmp al, 0x69	;Fine
	ja @f
	push eax
	push edx
	push ebx
	mov edi, dword [ESP+18]
	
	xor edx, edx
	mov eax, edi
	sub eax, 0xB8000
	mov ebx, 160
	div ebx		;Modulo in edx
	neg edx
	add edx, 158
	test byte [IsCaretActive], 1
	jz .NF
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi+edx+1], 4			;Nuovo caret
.NF:
	add edi, edx
	mov dword [ESP+18], edi
	
	pop ebx
	pop edx
	pop eax
	ret
@@:	cmp al, 0x6B	;Freccia sx
	ja @f
	mov edi, dword [ESP+6]
	test byte [IsCaretActive], 1
	jz .NX
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi-1], 4		;Nuovo caret
.NX:	sub dword [ESP+6], 2
	ret
@@:	cmp al, 0x6C	;Home
	ja @f
	push eax
	push edx
	push ebx
	mov edi, dword [ESP+18]
	
	xor edx, edx
	mov eax, edi
	sub eax, 0xB8000
	mov ebx, 160
	div ebx			;Modulo in edx
	sub edi, edx
	test byte [IsCaretActive], 1
	jz .NH
	rol byte [edi+edx+1], 4		;Elimina caret
	rol byte [edi+1], 4			;Nuovo caret
.NH:	
	mov dword [ESP+18], edi
	
	pop ebx
	pop edx
	pop eax
	ret
@@:	cmp al, 0x71	;Cose
	ja @f
	ret
@@:	cmp al, 0x72	;Freccia giu
	ja @f
	mov edi, dword [ESP+6]
	test byte [IsCaretActive], 1
	jz .NG
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi+161], 4		;Nuovo caret
.NG:	add dword [ESP+6], 160
	ret
@@:	cmp al, 0x74	;Freccia dx
	ja @f
	mov edi, dword [ESP+6]
	test byte [IsCaretActive], 1
	jz .ND
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi+3], 4		;Nuovo caret
.ND:	add dword [ESP+6], 2
	ret
@@:	cmp al, 0x75	;Freccia su
	ja @f
	mov edi, dword [ESP+6]
	test byte [IsCaretActive], 1
	jz .NS
	rol byte [edi+1], 4		;Elimina caret
	rol byte [edi-159], 4		;Nuovo caret
.NS:	sub dword [ESP+6], 160
	ret
@@:	ret

;==========================================================
d4:	ret

KS dd KS1	;70
KS1 db 'q1',0,0,0,'zsaw2',0,0,'cxde43',0,0,' vftr5',0,0,'nbhgy6',0,0,0,'mju78',0,0,',kio09',0,0,'./l;p-',0,0,0,"'",0,'[=',0,0,'`]',0,'\'
KS2 db 'Q!',0,0,0,'ZSAW@',0,0,'CXDE$#',0,0,' VFTR%',0,0,'NBHGY^',0,0,0,'MJU&*',0,0,'<KIO)(',0,0,'>/L:P_',0,0,0,'"',0,'{+',0,0,'~}',0,'?'
KS3 db 'Q1',0,0,0,'ZSAW2',0,0,'CXDE43',0,0,' VFTR5',0,0,'NBHGY6',0,0,0,'MJU78',0,0,',KIO09',0,0,'./L;P-',0,0,0,"'",0,'[=',0,0,'`]',0,'\'
KS4 db 'q!',0,0,0,'zsaw@',0,0,'cxde$#',0,0,' vftr%',0,0,'nbhgy^',0,0,0,'mju&*',0,0,'<kio)(',0,0,'>/l:p_',0,0,0,'"',0,'{+',0,0,'~}',0,'?'
KP1 db '1',0,'47',0,0,0,'0.2568',0,0,0,'+3-*9'

decoded db 0,0
