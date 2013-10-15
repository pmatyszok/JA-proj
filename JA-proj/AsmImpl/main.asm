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

CorrectGamma PROC bitmap : DWORD, size: DWORD, gamma: QWORD
	
loop:
	
	ret
CorrectGamma ENDP

END DllEntry 
