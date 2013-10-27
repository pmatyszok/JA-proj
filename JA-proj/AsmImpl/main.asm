.386 

.MODEL FLAT, STDCALL


OPTION CASEMAP:NONE

INCLUDE    \masm32\include\windows.inc 
INCLUDE		\masm32\include\user32.inc
INCLUDELIB	\masm32\lib\user32.lib

.DATA
Smth	DB	"Test2", 0
Result	QWORD	?
.CODE

DllEntry PROC hInstDLL:HINSTANCE, reason:DWORD, reserved1:DWORD

	mov eax, TRUE 
	ret

DllEntry ENDP

;-------------------------------------------------------------------------
; To jest pusta funkcja. Nic nie robi. Wstawi³em j¹ tutaj, aby ci pokazaæ,
; gdzie nale¿y umieszczaæ w³asne funkcje w bibliotece DLL
;-------------------------------------------------------------------------

TestFunction PROC
	invoke MessageBox, NULL, ADDR Smth, ADDR Smth, MB_OK
	ret
TestFunction ENDP

CorrectGamma PROC bitmap : DWORD, arrsize: DWORD, gamma: QWORD
	push ebp
	mov ebp, esp
	;mov eax, dword ptr bitmap
	add eax, arrsize
	mov ecx, eax 
	
	lea ebx, bitmap
	main_loop:
		push cx	;preserving loop var
		mov eax, ebx
		add eax, 10
		mov ebx, eax
		lea ebx, [ebx+1]
		pop cx	;preserving loop var
	loop main_loop
	leave
	ret
CorrectGamma ENDP

END DllEntry 
