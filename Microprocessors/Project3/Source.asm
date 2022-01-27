.386
.model flat, c

.const
;align16
RedFactor dword 2989
GreenFactor dword 5870
BlueFactor dword 1140
NextRowStep dword 8192

.code
bmptogray_conversion1 PROC
	push ebp
	mov ebp, esp		; save stack pointer for realtive call to arguments and local vars

	;mov ebx, [ebp+20]	; ebx points to 'output_gray'

	mov edi, 0			; edi increases to 'height'
heightLoop:
	cmp edi, [ebp+8]	; cmp edi with 'height'
	je afterHeightLoop

	;calc out_gray next row memory position
	mov eax, edi
	;mul [NextRowStep]	; j * NextRowStep
	shl eax, 13			; j * 8192
	mov ebx, [ebp+20]
	add ebx, eax		; ebx points to next row of 'output_grey'

	mov ecx, [ebp+16]	; ecx points to next row of 'input_color'
	add ecx, eax
	
	mov esi, 0			; esi increases to 'width'
widthLoop:
	cmp esi, [ebp+12]	; cmp esi with 'width'
	je afterWidthLoop

	; loop code
	mov eax, [RedFactor]
	movzx edx, byte ptr [ecx]
	mul edx						; input_color[x][y].rgbRed * 2989
	inc ecx						; ecx points to input_color[x][y].rgbGreen
	add [ebx], eax		

	mov eax, [GreenFactor]
	movzx edx, byte ptr [ecx]
	mul edx						; input_color[x][y].rgbRed * 2989
	inc ecx						; ecx points to input_color[x][y].rgbBlue
	add [ebx], eax		

	mov eax, [BlueFactor]
	movzx edx, byte ptr [ecx]
	mul edx						; input_color[x][y].rgbRed * 2989
	inc ecx						; ecx points to input_color[x][y].rgbReserved
	add [ebx], eax		
	
	; div 10000 for proper range [0-255]
	cmp dword ptr [ebx], 0
	jz afterDivision

	push ebx
	; division
	mov eax, [ebx]
	mov ebx, 10000
	div ebx

	pop ebx
	mov [ebx], eax
afterDivision:
	
	add ebx, 4					; next 'output_grey' array element
	inc ecx						; next 'input_color' array element

	; EOF loop code

	inc esi
	jmp widthLoop
afterWidthLoop:
	inc edi
	jmp heightLoop
afterHeightLoop:

	xor eax, eax		; return 0
	pop ebp
	
	ret
bmptogray_conversion1 endp
end