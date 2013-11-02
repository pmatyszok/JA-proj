.386 

.MODEL FLAT, C


OPTION CASEMAP:NONE
.nolist
INCLUDE    \masm32\include\windows.inc 
INCLUDE		\masm32\include\user32.inc
INCLUDELIB	\masm32\lib\user32.lib
.list
.DATA
Smth	DB	"Test2", 0
Result	QWORD	?
.CODE

DllEntry PROC  hInstDLL:HINSTANCE, reason:DWORD, reserved1:DWORD

	mov eax, TRUE 
	ret

DllEntry ENDP

;-------------------------------------------------------------------------
; To jest pusta funkcja. Nic nie robi. Wstawi³em j¹ tutaj, aby ci pokazaæ,
; gdzie nale¿y umieszczaæ w³asne funkcje w bibliotece DLL
;-------------------------------------------------------------------------

TestFunction PROC
	;invoke MessageBox, NULL, ADDR Smth, ADDR Smth, MB_OK
	ret
TestFunction ENDP


CorrectGamma PROC C bitmap : DWORD, arrsize: DWORD, gamma: DWORD
	push ebp
	mov ebp, esp
	push esi
	
	;
	;Params access:
	;bitmap first element is ebp + 12
	;bitmap size is ebp + 16
	;gamma is ebp + 20
	;

	sub esp, 24
	;
	;create local vars stack
	;local vars:
	;first - esp + 4
	;second - esp + 8
	;third - esp + 12
	;fourth - esp + 16
	;fifth - esp + 20
	;
	mov edx, 0
	finit
	main_loop:
		;
		;loads data from array
		;
		mov ebx, dword ptr[ebp + 12]
		lea ebx, [ebx + edx]
		;
		;load byte, fill with zeros to make FPU-edible
		;
		movzx eax, byte ptr[ebx]

		;byte to memory - fpu eats from memory, not from register
		mov dword ptr[esp + 8], eax

		;load 255 to memory - needed to rescale pixels
		xor eax, eax
		mov eax, 255
		mov dword ptr[esp + 12], eax
	
		;rescale pixel, clean rubbish
		fild dword ptr[esp + 12]
		fild dword ptr[esp + 8]
		fdiv st(0), st(1)
		fstp st(1)
		;
		;Loads gamma to FPU
		;
		mov eax, dword ptr[ebp + 20]
		mov dword ptr[esp + 4], eax
		fld dword ptr[esp + 4]

		;because fyl2x needs them in order: x, y (to perform x^y)
		fxch
	

		fyl2x
		; now st0 = y*log2(x)
	
		fld st
		frndint
		;now st0 = int[y*log2(x)] and st1 = y*log2(x)

		fsub st(1), st
		fxch
		;now st0 has the fractional part, st1 has the integral part
	
		f2xm1
		;st0 = 2^st0 - 1, st1 - integral part

		fld1
		fadd
		;st = 2^st0 
		fscale
		; st = x^y !!!
		fstp st(1)

		;rescale pixel back to 0-255 range
		fild dword ptr[esp + 12]
		fmul st(0), st(1)


		;pixels are integer number, so rounding rules apply
		;we will retrive control word, save if for later, modify
		;retrive our integer, load old cword and its done

		fnstcw word ptr[esp + 12]
		movzx eax, word ptr[esp + 12]
		or eax, 0C00h ; set Rounding Control bits to 11 = truncate (towards 0)
		mov dword ptr[esp + 16], eax
		fldcw word ptr[esp + 16]
		fistp dword ptr[esp + 4]
		fldcw word ptr[esp + 12]
		mov cl, byte ptr[esp + 4]
		mov byte ptr[ebx], cl

		inc edx
		mov eax, dword ptr[ebp + 16]
		cmp edx, eax
	jne main_loop


	pop esi
	leave
	ret
CorrectGamma ENDP

END DllEntry 
