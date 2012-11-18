; Piotr Truszkowski
;

section .data
; numery funkcji systemowych
	sys_exit      equ 1
	sys_read      equ 3 
	sys_write     equ 4
; jaki mamy bufor
	BufferLen     equ 1000h
; komunikat o bledzie
	StrError      db "BLAD",0xa
	StrErrorLen   equ $-StrError
; to co mozemy wczytac z wejscia
	ISNUMBER      equ 1
	ISSYMBOL      equ 2
	ISWHITE       equ 3
; systemy liczbowe
	BINSYS        equ 2
	DECSYS        equ 10
	HEXSYS        equ 16

section .bss
	Buffer:       resb BufferLen          ; bufor
	UsingSys:     resd 1                  ; jakiego systemu liczbowego uzywamy
	WhichSymbol:  resb 1                  ; jaki symbol wczytany(ISNUMBER, ISSYMBOL, ISWHITE)
	StackSize:    resd 1                  ; rozmiar stosu
	PrevRead:     resd 1                  ; poprzednio wczytane
	What:         resb 1                  ; pomocnicze
	Adress:       resd 1                  ; pomocnicze

section .text
	global _start

print_char:
	push eax
	push ebx
	push ecx
	push edx
	mov  ebx, 1
	mov  [What], al
 	mov  eax, sys_write
	mov  ecx, What
	mov  edx, 1
	int  0x80
	pop  edx
	pop  ecx
	pop  ebx
	pop  eax
	ret

print_digits:
	xor  edx, edx
	div  ebx
	push edx
	inc  ecx
	cmp  eax, 0
	je After
	call print_digits
	After:
	pop  eax
	cmp al, 9
	jle Not__Greater__Than__Ten
	add  al, 7
	Not__Greater__Than__Ten: 
	add  al, 48
	call print_char
	loope After
	ret	

print_int:
	push eax
	push ebx
	push ecx
	push edx
	cmp  eax, 0
	jge  GreaterThanZero
	mov  edx, eax
	mov  eax, '-'
	call print_char
	mov  eax, edx
	neg  eax
GreaterThanZero:
; tutaj wypis w zaleznosci od systemu liczbowego	
	push eax
	cmp dword [UsingSys], DECSYS
	je   DecSysType
	cmp dword [UsingSys], BINSYS
	je   BinSysType
	jmp EndSysType
DecSysType:
	mov  eax, '!'
	jmp OkSysType
BinSysType:
	mov  eax, '%'
OkSysType:
	call print_char
EndSysType:
	pop eax
	xor  ecx, ecx
	mov  ebx, [UsingSys]        ; w wybranym sytemie liczbowym
	call print_digits
	mov  al, 10                 ; znak konca linni
	call print_char
	pop  edx
	pop  ecx
	pop  ebx
	pop  eax
	ret

DecStack:
	push ebx
	mov  ebx, [StackSize]
	sub  ebx, 2
	mov  [StackSize], ebx
	cmp  ebx, 0
	jl near Some__Error 
	pop  ebx
	ret

ExitProc:                     ; Tutaj wychodzmy,
	mov  eax, sys_exit          ; poniewaz wczesniej ustawiamy kod wyscia,
	int  0x80                   ; wiec tutaj musimy tylko wywolac sys_exit
	ret

_start: 

; ustawiamy domyslne wartosci
	xor  eax, eax
	mov byte [WhichSymbol], 0
	mov dword [StackSize], 0
	mov dword [UsingSys], HEXSYS

Main:		                      ; Taka namiastka maina z c/c++...
	push eax
	mov  eax, sys_read
	xor  ebx, ebx  
	mov  ecx, Buffer	
	mov  edx, BufferLen		
	int  0x80

	cmp  eax, 0
	jl near Some__Error 
	
	mov  edx, Buffer
	mov  [Adress], edx
	add  [Adress], eax
	mov  ecx, eax
	cmp  eax, 0
	je near End__Of__File
	mov  [PrevRead], eax

	pop  eax
	xor  edx,edx
	xor  ebx,ebx

; Troche jak to i tamto...
	
; W eax siedzi sobie aktualna wartosc wczytywanej liczby a na StackSize pomietany jest
; rozmiar stosu, na ktory wrzucam wczytane (cale juz) liczby. Na dl trzymam wczytany znak
; i sprawdzam pokolei: czy cyfra, czy inny system, czy bialy znak, czy jakas operacja.

Read__Counter:

	xor  edx, edx
	mov  ebx, [Adress]
	sub  ebx, ecx
	mov  dl, [ebx]

; Sprawdzenie co to za znak wczytujemy...

	cmp dword [UsingSys], DECSYS
	je   DEC_Format
	cmp dword [UsingSys], BINSYS
	je   BIN_Format
	jmp  HEX_Format

HEX_Format:
	cmp dl, '0'
	jl  near Not__Digit
	cmp dl, 'F'
	jg  near Not__Digit
	cmp dl, 'A'
	jge HEX_Digit
	cmp dl, '9'
	jle HEX_Digit
	jmp Not__Digit
HEX_Digit:	
	cmp dl, '9'
	jle End_Format
	sub dl, 7
	jmp End_Format
	
DEC_Format:
	cmp dl, '0'
	jl near Not__Digit
	cmp dl, '9'
	jg near Not__Digit
	jmp End_Format
	
BIN_Format:
	cmp dl, '0'
	jl near Not__Digit
	cmp dl, '1'
	jg near Not__Digit
	jmp End_Format
	
End_Format:
	sub  dl, 48
	mov byte [WhichSymbol], ISNUMBER	 
	push edx
	mov  edx, [UsingSys]
	imul  edx
	jo near Some__Error
	pop  edx
	add  eax, edx
	jo near Some__Error
	jmp  End__Of__Symbol

; Byc moze trzeba sie przelaczyc na inny system liczbowy niz HEXSYS
Not__Digit:
	cmp  dl, '%'
	je   Number__Hasnt__Ended
	cmp  dl, '!'
	je   Number__Hasnt__Ended
	jmp Not__Switch
Number__Hasnt__Ended:                   ; jesli przed chwila byla liczba to wrzucamy ja na stos
	cmp byte [WhichSymbol], ISNUMBER
	jne Number__Has__Ended
	mov  ebx, [StackSize]
	inc  ebx
	mov  [StackSize], ebx
	push eax
	xor  eax, eax
	mov byte [WhichSymbol], ISWHITE
Number__Has__Ended:                    
	cmp  dl, '%'
	je   Switch__To__Bin
	cmp  dl, '!'
	je   Switch__To__Dec

Not__Switch:
	mov  dword [UsingSys], HEXSYS         ; bedzie cos innego niz 0,1,2,..,%,! czyli przeczamy sie na HEX
	cmp  dl, 9
	je   White__Space
	cmp  dl, 10
	je   White__Space
	cmp  dl, 32
	je   White__Space
	jmp  Not__White__Space

Switch__To__Bin:
	mov dword [UsingSys], BINSYS
	jmp End__Switch

Switch__To__Dec:
	mov dword [UsingSys], DECSYS
	jmp End__Switch
	
End__Switch:
	jmp End__Of__Symbol

White__Space:
	cmp  byte [WhichSymbol], ISWHITE
	jne  Tmp111
	jmp  End__Of__Symbol
	Tmp111:
	mov  ebx, [StackSize]
	inc  ebx
	mov  [StackSize], ebx
	push eax
	xor  eax, eax
	mov  byte [WhichSymbol], ISWHITE
	jmp End__Of__Symbol	

Not__White__Space:
	mov  bl, [WhichSymbol]
	cmp  bl, ISWHITE
	jne WithoutSpace
	jmp WithSpace
WithoutSpace:                           ; ups niebylo spacji
	mov  ebx, [StackSize]
	inc  ebx
	mov  [StackSize], ebx
	push eax
	xor  eax, eax
WithSpace:                              ; juz jest spacja
	mov byte [WhichSymbol], ISSYMBOL
	
; no to pozostaje do sprawdzenie czy mamy do czynienia z jakims +,-,*,/
	cmp  dl, '+'
	jne  Not__Plus
	call DecStack
	pop  edx
	pop  eax
	add  eax, edx
	jo near Some__Error
	jmp End__Of__Operation

Not__Plus:
	cmp  dl, '*'
	jne  Not__Multi
	call DecStack
	pop  edx
	pop  eax
	imul edx
	jo near Some__Error
	jmp End__Of__Operation

Not__Multi:
	cmp  dl, '-'
	jne  Not__Minus
	call DecStack
	pop  edx
	pop  eax
	sub  eax, edx
	jo near Some__Error
	jmp End__Of__Operation

Not__Minus:
	cmp  dl, '/'
	jne near Some__Error
	call DecStack
	pop  ebx
	pop  eax
	xor  edx, edx
	cdq
	cmp  ebx, 0
	je near Division__By__Zero
	idiv  ebx
	jo near Some__Error
	jmp End__Of__Operation

End__Of__Operation:                     ; po operacji +,-,*,/ trzeba 'wstawic' spacje
	mov  ebx, [StackSize]
	inc  ebx
	mov  [StackSize], ebx
	push eax
	xor  eax, eax
	mov byte [WhichSymbol], ISWHITE
	mov dword [UsingSys], HEXSYS

End__Of__Symbol: 
	inc  ebx
	dec  ecx
	cmp  ecx, 0
	jne near Read__Counter
	mov  ebx, [PrevRead]
	cmp  ebx, BufferLen
	je  near Main

Finish:
	pop  eax
	mov  ebx, [StackSize]
	cmp  ebx, 1
	jne  Some__Error                      ; oho zle, cos zostalo nam na stosie !
                                        ; wpp tośmy otrzymali
	mov dword [UsingSys], HEXSYS          ; szesnastkowo, 
	call print_int		          
	mov dword [UsingSys], DECSYS          ; dziesietnie, 
	call print_int		          
	mov dword [UsingSys], BINSYS          ; i na koniec binarnie
	call print_int		          
	mov  ebx, 0                           ; jeszcze ustawienie kodu wyjscia na 0...
	call ExitProc                         ; i papa

Some__Error:
	mov edx, StrErrorLen                  ; komunikat stosowny
	mov ecx, StrError    
	mov ebx, 1                            ; kod bledu ustawiamy na 1
	mov eax, 4
	int 0x80
	call ExitProc                         ; i wyjscie

Division__By__Zero:                     ; na tyle wyroznialny blad, ze doczekal sie tutaj miejsca
	jmp near Some__Error

End__Of__File:                          ; read przeczytal 0 bajtow --- konczymy!
	pop eax
	jmp near Finish
