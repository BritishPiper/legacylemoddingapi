.code

SetupShadowSpaceEx proc
	pop rax
	add rsp, 40
	push r11
	push r10
	sub rsp, 40
	push rax
	ret
SetupShadowSpaceEx endp

ClearShadowSpaceEx proc
	pop r10
	pop r11
	push [rax + 0]
	mov rcx, [rax + 8]
	mov rdx, [rax + 16]
	mov r8, [rax + 24]
	mov r9, [rax + 32]
	mov rax, [rax + 40]
	ret
ClearShadowSpaceEx endp

ClearShadowSpaceEx2 proc
	pop r10
	pop r11
	push [rax + 0]
	mov rcx, [rax + 8]
	mov rdx, [rax + 16]
	mov r8, [rax + 24]
	mov r9, [rax + 32]
	mov rdi, [rax + 48]
	mov rax, [rax + 40]
	ret
ClearShadowSpaceEx2 endp

SetupShadowSpace proc
	ret
SetupShadowSpace endp

ClearShadowSpace proc
	ret
ClearShadowSpace endp

MovRcxRax proc
	mov rcx,rax
	ret
MovRcxRax endp

MovRcxRbx proc
	mov rcx,rbx
	ret
MovRcxRbx endp

MovRcxRdi proc
	mov rcx,rdi
	ret
MovRcxRdi endp

MovRdxRax proc
	mov rdx,rax
	ret
MovRdxRax endp

MovRdxRbx proc
	mov rdx,rbx
	ret
MovRdxRbx endp

MovRdxR15 proc
	mov rdx,r15
	ret
MovRdxR15 endp

MovR8Rax proc
	mov r8, rax
	ret
MovR8Rax endp

MovR8Rdi proc
	mov r8, rdi
	ret
MovR8Rdi endp

MovR9R13 proc
	mov r9, r13
	ret
MovR9R13 endp

AddRdi30 proc
	add rdi, 30h
	ret
AddRdi30 endp

end
