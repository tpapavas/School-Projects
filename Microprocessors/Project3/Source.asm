.386
.model flat, c

.const
;align16
RedFactor dword 2989
GreenFactor dword 5870
BlueFactor dword 1140
NextRowStep dword 8192
S1 dword -1,0,1,-2,0,2,-1,0,1
S2 dword -1,-2,-1,0,0,0,1,2,1
ZERO dword 0
THRES dword 10
temp real8 ?

.code
bmptogray_conversion1 PROC
	push ebp
	mov ebp, esp		; save stack pointer for realtive call to arguments and local vars
	push esi
	push edi
	pushfd


	;mov ebx, [ebp+20]	; ebx points to 'output_gray'

	mov edi, 0			; edi increases to 'height'
heightLoop:
	cmp edi, [ebp+8]	; cmp edi with 'height'
	je afterHeightLoop

	;calc out_gray next row memory position
	mov eax, edi
	;mul [NextRowStep]	; j * NextRowStep
	shl eax, 2			; j * 4
	mov ebx, [ebp+20]
	add ebx, eax		; ebx points to next row of 'output_gray'

	mov ecx, [ebp+16]	; ecx points to next row of 'input_color'
	add ecx, eax
	
	mov esi, 0			; esi increases to 'width'
widthLoop:
	cmp esi, [ebp+12]	; cmp esi with 'width'
	je afterWidthLoop

	; loop code
	mov eax, [BlueFactor]
	movzx edx, byte ptr [ecx]
	mul edx						; input_color[x][y].rgbRed * 2989
	inc ecx						; ecx points to input_color[x][y].rgbGreen
	add [ebx], eax		

	mov eax, [GreenFactor]
	movzx edx, byte ptr [ecx]
	mul edx						; input_color[x][y].rgbRed * 2989
	inc ecx						; ecx points to input_color[x][y].rgbBlue
	add [ebx], eax		

	mov eax, [RedFactor]
	movzx edx, byte ptr [ecx]
	mul edx						; input_color[x][y].rgbRed * 2989
	inc ecx						; ecx points to input_color[x][y].rgbReserved
	add [ebx], eax		
	
	; div 10000 for proper range [0-255]
	push ebx
	; division
	mov eax, [ebx]
	mov ebx, 10000
	div ebx

	pop ebx
	mov [ebx], eax
afterDivision:
	
	add ebx, 8192				; next 'output_gray' array element
	inc ecx						; next 'input_color' array element
	add ecx, 8188

	; EOF loop code

	inc esi
	jmp widthLoop
afterWidthLoop:
	inc edi
	jmp heightLoop
afterHeightLoop:

	xor eax, eax		; return 0

	popfd				; restore eflag
	pop edi
	pop esi
	pop ebp
	
	ret
bmptogray_conversion1 endp


sobel_detection1 proc
	push ebp
	mov ebp, esp
	push esi
	push edi
	pushfd

	;;testing
	;mov eax, [ebp+24]
	;add eax, 16392
	;mov [ebp+24], eax
	;;;;;;;;;;


	mov edi, 1				; edi increases to 'width'-1
	dec dword ptr [ebp+12]	; 'width'-1
	dec dword ptr [ebp+8]	; 'height'-1
widthLoop:
	cmp edi, [ebp+12]		; cmp edi with 'width'-1
	je afterWidthLoop

	;calc out_gray next row memory position
	mov eax, edi
	;mul [NextRowStep]	; i * NextRowStep
	shl eax, 11			; i * 2048
	mov ebx, [ebp+20]
	add ebx, eax		; ebx points to next row of 'output_ee_image'

	shl eax, 2			; i * 8192
	mov ecx, [ebp+16]	
	add ecx, eax		; ecx points to next row of 'input_gray_image'

	;;testing
	;mov eax, [ebp+24]
	;add eax, 16
	;mov [ebp+24], eax
	;;;;;;;;;

	
	inc ebx
	add ecx, 4
	mov esi, 1			; esi increases to 'height'-1
heightLoop:
	cmp esi, [ebp+8]	; cmp esi with 'height'-1
	je afterHeightLoop

	; loop code
	;; Gx calculation ;;
	sub ecx, 8192				; x-1
	sub ecx, 4					; y-1
	fild dword ptr [ecx]		; gray_image[x-1][y-1]
	fimul [S1]					; gray_image[x-1][y-1] * S1[0][0]

	add ecx, 4					; y
	fild dword ptr [ecx]		; gray_image[x-1][y]
	fimul [S1+4]				; gray_image[x-1][y] * S1[0][1]
	faddp st(1), st(0)			; part sum

	add ecx, 4					; y+1
	fild dword ptr [ecx]		; gray_image[x-1][y+1]
	fimul [S1+8]				; gray_image[x-1][y+1] * S1[0][2]
	faddp st(1), st(0)

	add ecx, 8192				; x
	sub ecx, 8					; y-1
	fild dword ptr [ecx]		; gray_image[x][y-1]
	fimul [S1+12]				; gray_image[x][y-1] * S1[1][0]
	faddp st(1), st(0)

	add ecx, 4					; y
	fild dword ptr [ecx]		; gray_image[x][y]
	fimul [S1+16]				; gray_image[x][y] * S1[1][1]
	faddp st(1), st(0)

	add ecx, 4					; y+1
	fild dword ptr [ecx]		; gray_image[x][y]
	fimul [S1+20]				; gray_image[x][y] * S1[1][2]
	faddp st(1), st(0)

	add ecx, 8192				; x+1
	sub ecx, 8					; y-1
	fild dword ptr [ecx]		; gray_image[x+1][y-1]
	fimul [S1+24]				; gray_image[x+1][y-1] * S1[2][0]
	faddp st(1), st(0)

	add ecx, 4					; y
	fild dword ptr [ecx]		; gray_image[x+1][y]
	fimul [S1+28]				; gray_image[x+1][y] * S1[2][1]
	faddp st(1), st(0)

	add ecx, 4					; y+1
	fild dword ptr [ecx]		; gray_image[x+1][y+1]
	fimul [S1+32]				; gray_image[x+1][y+1] * S1[2][2]
	faddp st(1), st(0)
	
	fmul st(0), st(0)			; Gx^2

	sub ecx, 8192				; x
	sub ecx, 4					; y

	;; Gy calculation ;;
	sub ecx, 8192				; x-1
	sub ecx, 4					; y-1
	fild dword ptr [ecx]		; gray_image[x-1][y-1]
	fimul [S2]					; gray_image[x-1][y-1] * S1[0][0]

	add ecx, 4					; y
	fild dword ptr [ecx]		; gray_image[x-1][y]
	fimul [S2+4]				; gray_image[x-1][y] * S1[0][1]
	faddp st(1), st(0)			; part sum

	add ecx, 4					; y+1
	fild dword ptr [ecx]		; gray_image[x-1][y+1]
	fimul [S2+8]				; gray_image[x-1][y+1] * S1[0][2]
	faddp st(1), st(0)

	add ecx, 8192				; x
	sub ecx, 8					; y-1
	fild dword ptr [ecx]		; gray_image[x][y-1]
	fimul [S2+12]				; gray_image[x][y-1] * S1[1][0]
	faddp st(1), st(0)

	add ecx, 4					; y
	fild dword ptr [ecx]		; gray_image[x][y]
	fimul [S2+16]				; gray_image[x][y] * S1[1][1]
	faddp st(1), st(0)

	add ecx, 4					; y+1
	fild dword ptr [ecx]		; gray_image[x][y]
	fimul [S2+20]				; gray_image[x][y] * S1[1][2]
	faddp st(1), st(0)

	add ecx, 8192				; x+1
	sub ecx, 8					; y-1
	fild dword ptr [ecx]		; gray_image[x+1][y-1]
	fimul [S2+24]				; gray_image[x+1][y-1] * S1[2][0]
	faddp st(1), st(0)

	add ecx, 4					; y
	fild dword ptr [ecx]		; gray_image[x+1][y]
	fimul [S2+28]				; gray_image[x+1][y] * S1[2][1]
	faddp st(1), st(0)

	add ecx, 4					; y
	fild dword ptr [ecx]		; gray_image[x+1][y+1]
	fimul [S2+32]				; gray_image[x+1][y+1] * S1[2][2]
	faddp st(1), st(0)
	
	sub ecx, 8192				; x
	sub ecx, 4					; y

	fmul st(0), st(0)			; Gy^2
	;; eof Gy calculation ;;
	faddp						; Gx^2 + Gy^2
	fsqrt						; sqrt(Gx^2 + Gy^2)

	;;;for testing;;;
	;push ebx
	;mov ebx, [ebp+24] 
	;fst real8 ptr [ebx]
	;pop ebx

	fild dword ptr [ebp+24]		; load THRESHOLD
	;fild [THRES]
	fcompp						; cmp(Gx^2 + Gy^2, THRESHOLD) and double pop
	;ficomp dword ptr [ebp+24]
	fnstsw ax					; load status reg to ax
	sahf						; store ah into flags

	jbe white
	mov al, 0
	mov [ebx], al 
	jmp afterColorSet
white:	
	mov al, 0FFh
	mov [ebx], al 
afterColorSet:
;	fild [ZERO]
;	fild [ZERO]
;	fild [ZERO]
;	fild [ZERO]

	inc ebx						; next 'output_ee' array element
	add ecx, 4					; next 'input_gray' array element
	; EOF loop code
	
	;;;testing
	;mov eax, [ebp+24]
	;add eax, 8
	;mov [ebp+24], eax
	;;;;;;;;;;

	inc esi
	jmp heightLoop
afterHeightLoop:
	inc edi
	jmp widthLoop
afterWidthLoop:

	xor eax, eax		; return 0

	popfd
	pop edi
	pop esi
	pop ebp

	ret
sobel_detection1 endp


end