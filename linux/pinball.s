	.file	"pinball.bc"
	.text
	.globl	main
	.align	16, 0x90
	.type	main,@function
main:                                   # @main
	.cfi_startproc
# BB#0:                                 # %entry
	pushq	%rax
.Ltmp1:
	.cfi_def_cfa_offset 16
	xorl	%edi, %edi
	callq	exit
.Ltmp2:
	.size	main, .Ltmp2-main
	.cfi_endproc


	.section	".note.GNU-stack","",@progbits
