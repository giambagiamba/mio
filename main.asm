
	use16
start:
	mov ax, 0x0000
	mov ss, ax
	mov ds, ax
	mov sp, 0xffff
	addr_2sect=0x7E00		;Quivi verrà copiato il secondo settore del floppy
	segm_2sect=(addr_2sect shr 16)	;E' dopo la regione del Bootloader
	offs_2sect=addr_2sect-(segm_2sect shl 16)
	mov ax, segm_2sect
	mov es, ax
	mov bx, offs_2sect
;=====================LETTURA FLOPPY========================
	mov ah, 0x02
	mov al, 10	;Settori da leggere
	mov ch, 0	;Cylinder
	mov dh, 0	;Head
	mov cl, 2	;Sector
	mov dl, 0	;#Drive - 1^ floppy
	int 0x13
	jc IDLER
	;========PULITURA SCHERMO===================
	xor ah, ah
	mov al, 2
	int 0x10
	mov ah, 1
	mov ch, 0x3F
	int 0x10
	
	jmp segm_2sect:offs_2sect
	
IDLER:
	hlt
	jmp IDLER
	
	times 510- ($-$$) db 0
	dw 0xAA55
		
;======================SECONDO SETTORE======================
	;Empiricamente si trova che la modalità di default è 80*25 (pitch 160)
	
	;;;;;;;;;;;;;;;;;;PARAMETRI & LORO PASSAGGIO & REGISTRI VOLATILI;;;;;;;;;;;;
	;; al->Registro per in e out                                              ;;
	;; eax->Registro di ritorno (e sottoregistri)                             ;;
	;; ebx->Puntatore video, puo essere incrementato da funzioni di stampa    ;;
	;; ch->ColorAttributes	cl->riservato per valore da stampare (mov word)   ;;
	;; edx->Puntatore stringa                                                 ;;
	;;                                                                        ;;
	;;                                                                        ;;
	;; Volatili->eax, ebx (solo per stampa), ecx-ch, edx, ebp                 ;;
	;; Non volatili->ch, esi, edi                                             ;;
	;;                                                                        ;;
	;;                                                                        ;;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	
	org addr_2sect
	
	cli	
	mov	al, 11111110b	;Slave
	out	0xa1, al
	Wait
	mov	al, 11111000b	;Master: tastiera, timer e slave
	out	0x21, al
	
;========================RICERCA MEMORIA========================
MEM:
	pushf
	prememmap=0x2000
	mov di, prememmap shr 4
	mov es, di
	xor si, si
	xor di, di
	xor ebx, ebx
	mov edx, 0x534d4150
	mov eax, 0x0000e820
	mov ecx, 24
	mov dword [es:di+20], 1
	int 0x15
	jc .F1
	mov edx, 0x534d4150
	cmp eax, edx
	jne .F1
	test ebx, ebx
	jz .F1
	
.R1:	cmp dword [es:di+16], 1
	jne @f
	add di, 24
	add si, 1
@@:	mov eax, 0x0000e820
	mov ecx, 24
	int 0x15
	jc .F1
	test ebx, ebx
	jnz .R1
.F1:	
	mov dword [MEMMAP], prememmap
	mov word [MEMMAP+4], si
	popf
	
	
;============PASSAGGIO ALLA MODALITA PROTETTA===================
PM:	mov ch, 2			;verde - In ch attributi colore
	call BuildGDT
	mov esi, gdtr
	lgdt [esi]
	call BuildIDT
	mov esi, idtr
	lidt [esi]
	mov eax, cr0
	or eax, 1
	and eax, 10011111111111111111111111111111b	;Caching
	mov cr0, eax
	jmp pword 0x08:E

include 'tables.inc'
;=====================^ 16bit ^========|       |=================
;=====================|       |========v 32bit v=================
use32
include 'functions.inc'
E:
	mov ax, 0x10	;Tutti data segments
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov esp, 0x0200ffff
	
	call TestGate
	test al, al
	jnz @f
	call EnableGate
	call TestGate
	test al, al
	jnz @f
	mov ebx, (0xB8000+24*160+114)
	mov edx, str_gate
	call print
	jmp @f
@@:	
;=====================PROVE CON IL 8259A========================
	call Init8259A
	; sti

;==================CREAZIONE AMBIENTE GRAFICO===================
	call DrawGUI

;===================INIZIALIZZAZIONE TASTIERA===================
	call Init8242_KBD

;===================INIZIALIZZAZIONE RTC========================
	call InitRTC
	
;=================PREPARAZIONE MULTITASKING=====================	
	mov ebx, 0xB8000
	
	mov esi, 0x300000
	mov dword [TASKLIST], esi	;Dove sta la regione
	mov word [TASKLIST+4], 0		;Processo corrente
	mov word [TASKLIST+6], 2		;2 processi per ora
	mov dword [esi], TASK1
	mov byte [esi+4], 0
	mov dword [esi+8], TASK2
	mov byte [esi+12], 0
	
	mov esi, TASK1
	mov edi, TASK2
	mov dword [edi+12], ecx			;Per gli attributi colore
	mov dword [esi+28], esp
	mov dword [edi+28], 0x02ffffff	;esp di task2
	pushf
	mov eax, dword [esp]
	or eax, 1000000000b
	mov dword [edi+36], eax
	sti
	jmp dword [TASK1]
	
	
	
TASK1:
	dd Entry1,0,0,0,0,0,0,0,0,0
TASK2:
	dd Entry2,0,0,0,0,0,0,0,0,0
	
Entry1:
	pusha
	xor ebp, ebp
	mov ebx, 0xB8000+160*2+4
	mov edi, dword [MEMMAP]
	mov si, word [MEMMAP+4]
@@:	mov eax, dword [edi]
	call printoxd
	add ebx, 18
	mov eax, dword [edi+8]
	add ebp, eax	;For memory size
	call printoxd
	add ebx, 18
	mov eax, dword [edi+16]
	call printoxd
	add ebx, 160-36
	add edi, 24
	sub si, 1
	jnz @b
	
	mov dword [MEM_BITMAP], ebp
	mov eax, ebp
	call printoxd
	
	mov edi, dword [MEMMAP]
@@:	mov eax, dword [edi]
	cmp eax, 0x100000
	jae @f
	add edi, 24
	jmp @b	;Bitmap iniziano nella prima parte oltre 1M
	;=========CREAZIONE BITMAP=========
@@:	mov [MEM_BITMAP+4], eax	;bitmap delle pagine
	mov edx, -1
	;mov ebp, dword [edi+8]
	mov ecx, ebp
	shr ecx, 12	;pagine
	mov ebx, ecx
	shr ecx, 5		;ogni bit è una pagina
	and ebx, 11111b	;bit avanzanti
@@:	mov dword [eax], edx
	add eax, 4
	sub ecx, 1
	jnz @b
	mov cl, 32
	sub cl, bl
	shr edx, cl
	mov dword [eax], edx
	
	add eax, 4
	mov [MEM_BITMAP+8], eax	;bitmap dei 64k
	mov edx, -1
	mov ecx, ebp
	shr ecx, 16	;blocchi da 64k
	mov ebx, ecx
	shr ecx, 5		;ogni bit è un blocco
	and ebx, 11111b	;bit avanzanti
@@:	mov dword [eax], edx
	add eax, 4
	sub ecx, 1
	jnz @b
	mov cl, 32
	sub cl, bl
	shr edx, cl
	mov dword [eax], edx
	
	add eax, 4
	mov [MEM_BITMAP+12], eax	;bitmap dei 1M
	mov edx, -1
	mov ecx, ebp
	shr ecx, 20	;blocchi da 1M
	mov ebx, ecx
	shr ecx, 5		;ogni bit è un blocco
	and ebx, 11111b	;bit avanzanti
@@:	mov dword [eax], edx
	add eax, 4
	sub ecx, 1
	jnz @b
	mov cl, 32
	sub cl, bl
	shr edx, cl
	mov dword [eax], edx
	
	popa
	
	mov ebx, 0xb8000+20*160
	xor al, al
@@:	add al, 1
	call printoxb
	hlt
	jmp @b

Entry2:
	mov ebx, 0xb8000+20*160+32
	xor al, al
@@:	add al, 1
	call printoxb
	hlt
	jmp @b
	
	
	
	;MODELLO MULTITASKING: regione puntata da TASKLIST[0] contiene vettore di doppie dword. La prima dword contiene puntatore a regione ove è memorizzato lo stato del task; La seconda è di flags, di cui il bit più basso indica stato "ready" (0) o "waiting" (1); TASKLIST[4] contiene numero indicante processo correntemente attivo nella word bassa, numero totale di processi nella word alta.
	;Strutture di stato dei processi sono siffatte: dd EIP, EAX, EBX, ECX, EDX, ESI, EDI, ESP, EBP, EFLAGS. L'istruzione da cui riprendere è puntata da EIP. TASK1 rappresenta il kernel con IDLE e tutto quanto. TASK2 rappresenta il terminale. Quando un processo vuole cedere il posto può farlo eseguendo int 41, ove c'è il meccanismo di switching; in ogni caso anche il RTC impone lo switching.
	
	
	
	
	
	times 168000h-($+512-$$) db 0	;<-512 è la taglia della mbr


	; mov dword [MEM_BITMAP], ebp
	
	; mov eax, dword [MEM_BITMAP]
	; call printoxd
	
	; xor ebp, ebp
	; mov edi, dword [MEMMAP]
	; mov si, word [MEMMAP+4]
	; mov eax, dword [MEM_BITMAP]
	; push eax
; .L1:	mov eax, dword [edi]
	; cmp eax, dword [esp]
	; je .L2
	; add edi, 24
	; sub si, 1
	; jz .L4
	; jmp .L1
; .L2:	mov eax, dword [edi+8]
	; add dword [esp], eax			;Fine della voce
	; shr eax, 15					;Pagine, una in ogni bit
	;; mov ebp, dword [MEM_BITMAP+4]
	; add ebp, eax					;Conteggio della memoria disponibile
	; mov edx, dword [MEM_BITMAP+ebp]
; .L3:	mov dword [edx], ebp
	; add edx, 4
	; sub eax, 4
	; ja .L3
	; jmp .L1
; .L4:	mov dword [MEM_BITMAP+4], ebp
	; add esp, 4
	
	; add ebx, 160
	; mov eax, dword [MEM_BITMAP+4]
	; shl eax, 15
	; call printoxd
