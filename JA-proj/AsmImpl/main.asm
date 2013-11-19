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
	push ebp ;create function stack-frame
	mov ebp, esp ; save return adress
	
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

	mov edx, 0 ; loop counter
	
	mov eax, 255 ;load 255 to memory - needed to rescale pixels
	mov dword ptr[esp + 12], eax ;transfer 255 to memory so FPU can fetch it

	main_loop: ;main program loop begins
		
		finit ; initilize FPU
		;
		;loads data from array
		;
		mov ebx, dword ptr[ebp + 12] ;loads array first element
		lea ebx, [ebx + edx] ;loads effective addres of curren array element
		
		;
		;load byte, fill with zeros to make FPU-edible
		;
		movzx eax, byte ptr[ebx] ;loads byte and aligns it with zeros

		
		mov dword ptr[esp + 8], eax ;move current byte to memory - fpu takes from memory, not from register
	
		;rescale pixel

		fild dword ptr[esp + 12] ;load 255 to FPU
		fild dword ptr[esp + 8] ;load current byte to FPU
		fdiv st(0), st(1) ;rescale pixel to 0-1
		
		fld dword ptr[ebp + 20] ;Loads gamma value to FPU

		;
		; pow() implementation begins
		;
		
		fxch ;exchange st0 with st1 because fyl2x needs them in order: x, y (to perform x^y)
	

		fyl2x		; now st0 = y*log2(x)
	
		fld st	;duplicate st0, so st0 = st1
		frndint	;now st0 = int[y*log2(x)] and st1 = y*log2(x)

		fsub st(1), st ; st0 has integral part, st1 is fractional part
		fxch		;now st0 has the fractional part, st1 has the integral part
	
		f2xm1		;st0 = 2^st0 - 1, st1 - integral part

		fld1	;load 1 to FPU
		fadd	;st = 2^st0 
		fscale  ; st = x^y !!! 

		;
		; pow() implementation ends
		;

		;
		;rescale pixel back to 0-255 range
		;

		fild dword ptr[esp + 12] ;load 255 to FPU
		fmul				     ;st0 is calculated pixel value in 0-255 range again
		
		
		;pixels are integer number, so rounding rules apply
		;we will retrive control word, save if for later, modify
		;retrive our integer, load old cword and its done

		fnstcw word ptr[esp + 20] ;retrive control word, store it in memory
		movzx eax, word ptr[esp + 20] ;transfer control word to acumulator, align it with zeroes
		or eax, 0C00h ; set Rounding Control bits to 11 = truncate (towards 0)
		mov dword ptr[esp + 16], eax ; save modified control word in memory
		fldcw word ptr[esp + 16] ; load modified control word to FPU
		fistp dword ptr[esp + 8] ; retrive calculated integer pixel value to memory
		fldcw word ptr[esp + 20] ; load unmodified (old) control word back to FPU

		;
		;transfer back to image
		;
		mov cl, byte ptr[esp + 8] ; move lest significant bits of retrivied pixel value to cl
		mov byte ptr[ebx], cl	  ;store it in its place in image
		
		add edx, 1 ;increase loop variable
		mov eax, dword ptr[ebp + 16]  ;load array size
		cmp edx, eax ; compare current loop variable with array size
	jne main_loop ; if size is not reached - continue loop

	leave ;clean up stack frame
	ret ;return
CorrectGamma ENDP

END DllEntry 
