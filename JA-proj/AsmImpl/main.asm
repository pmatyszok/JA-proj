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
; To jest pusta funkcja. Nic nie robi. Wstawi�em j� tutaj, aby ci pokaza�,
; gdzie nale�y umieszcza� w�asne funkcje w bibliotece DLL
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
