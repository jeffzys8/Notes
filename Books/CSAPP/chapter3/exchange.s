	.file	"exchange.c"
	.text
	.globl	exchange
	.type	exchange, @function
exchange:
.LFB0:
	.cfi_startproc
	endbr32
	movl	4(%esp), %eax
	movl	(%eax), %edx
	movl	8(%esp), %ecx
	movl	%ecx, (%eax)
	movl	%edx, %eax
	ret
	.cfi_endproc
.LFE0:
	.size	exchange, .-exchange
	.ident	"GCC: (Ubuntu 9.3.0-17ubuntu1~20.04) 9.3.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 4
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 4
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 4
4:
