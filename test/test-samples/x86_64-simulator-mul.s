	.set	USE_STACK, 1
	.include "x86_64-simulator-header.s"
start:
	# 8 bits operands
	mov 	$0x11, %al
	mov 	%al, %bl	
	mul	%bl
	call	chk1	
	cmp	$0x121, %ax
	jne	error

	mov 	$0x2, %al
	mov 	%al, %bl
	mul	%bl
	call	chk0	
	cmp	$0x4, %ax
	jne	error

	mov 	$0xFE, %bl
	mul	%bl
	call	chk1	
	cmp	$0x03F8, %ax
	jne	error

	mov 	$0xFE, %bl
	mul	%bl
	call	chk1	
	cmp	$0xF610, %ax
	jne	error

	# 16 bits operands
	mov 	$0x1111, %ax
	mov 	%ax, %bx
	mul	%bx
	call	chk1	
	cmp	$0x4321, %ax
	jne	error
	cmp	$0x123, %dx
	jne	error

	mov	$0x1234, %ax
	mov 	$0x3, %bx
	mul	%bx
	call	chk0	
	cmp	$0x369C, %ax
	jne	error
	test	%dx, %dx
	jnz	error
	
	mov 	$0xFFFD, %bx
	mul	%bx
	call	chk1	
	cmp	$0x5C2C, %ax
	jne	error
	cmp	$0x369b, %dx
	jne	error

	mov	$0xF234, %ax
	mov 	$0xFFFD, %bx
	mul	%bx
	call	chk1	
	cmp	$0x2964, %ax
	jne	error
	cmp	$0xF231, %dx
	jne	error	

	# 32 bits operands
	mov 	$0x11111111, %eax
	mov 	%eax, %ebx
	mul	%ebx
	call	chk1	
	cmp	$0x87654321, %eax
	jne	error
	cmp	$0x1234567, %edx
	jne	error

	mov	$0x12345678, %eax
	mov 	$0x3, %ebx
	mul	%ebx
	call	chk0	
	cmp	$0x369D0368, %eax
	jne	error
	test	%edx, %edx
	jnz	error

	mov 	$0xFFFFFFFD, %ebx
	mul	%ebx
	call	chk1	
	cmp	$0x5C28F5C8, %eax
	jne	error
	cmp	$0x369D0367, %edx
	jne	error

	mov	$0xF2345678, %eax
	mov 	$0xFFFFFFFD, %ebx
	mul	%ebx
	call	chk1	
	cmp	$0x2962FC98, %eax
	jne	error
	cmp  	$0xF2345675, %edx
	jne	error	

	.include "x86_64-simulator-end.s"

chk1:
	jnc	error
	jno	error
	ret

chk0:
	jc	error
	jo	error
	ret
	