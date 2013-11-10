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

CorrectGamma PROC C bitmap : DWORD, arrsize: DWORD, gamma: DWORD
	push ebp
	mov ebp, esp
	
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
	;third - esp + 12  - this holds 255
	;fourth - esp + 16
	;fifth - esp + 20
	;
	mov edx, 0
	;load 255 to memory - needed to rescale pixels
	mov eax, 255
	mov dword ptr[esp + 12], eax
	main_loop:
		finit
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
	
		;rescale pixel
		fild dword ptr[esp + 12]
		fild dword ptr[esp + 8]
		fdiv st(0), st(1)

		;
		;Loads gamma to FPU
		;
		fld dword ptr[ebp + 20]

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

		;rescale pixel back to 0-255 range
		fild dword ptr[esp + 12]
		fmul 
		
		
		;pixels are integer number, so rounding rules apply
		;we will retrive control word, save if for later, modify
		;retrive our integer, load old cword and its done

		fnstcw word ptr[esp + 20]
		movzx eax, word ptr[esp + 20]
		or eax, 0C00h ; set Rounding Control bits to 11 = truncate (towards 0)
		mov dword ptr[esp + 16], eax
		fldcw word ptr[esp + 16]
		fistp dword ptr[esp + 8]
		fldcw word ptr[esp + 20]

		;transfer value
		mov cl, byte ptr[esp + 8]
		mov byte ptr[ebx], cl
		
		add edx, 1
		mov eax, dword ptr[ebp + 16]
		cmp edx, eax
	jne main_loop

	leave
	ret 
CorrectGamma ENDP

END DllEntry 
