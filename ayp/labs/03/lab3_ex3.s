	.file	1 "lab3_ex3.c"
	.section .mdebug.eabi32
	.previous
	.section .gcc_compiled_long32
	.previous
	.gnu_attribute 4, 1
	.text
	.align	2
	.globl	main
	.set	nomips16
	.ent	main
	.type	main, @function

main:
	.frame	$sp,0,$31		# vars= 0, regs= 0/0, args= 0, gp= 0
	.mask	0x00000000,0
	.fmask	0x00000000,0
	lui	$6,%hi(source)
	lw	$3,%lo(source)($6)
	beq	$3,$0,$L2
	lui	$7,%hi(dest)
	addiu	$7,$7,%lo(dest)
	addiu	$6,$6,%lo(source)
	move	$2,$0

$L3:								#This is the loop
	addu	$5,$7,$2								
	addu	$4,$6,$2				#$2 is the incrementor (shows how much the offset is in the array)
	sw	$3,0($5)
	lw	$3,4($4)					#Loads the number just taken from source into dest 
	addiu	$2,$2,4
	bne	$3,$0,$L3

	#Description of registers:
	#	$2 keeps track of how much to offset each address by from the original array pointer 
	#	$7 and $6 are the registers that contain the original array pointers
	
	#	$5 and $4 then use that number for the assignment later on... equivalent of arr[k], except in both cases they are identical here
	
	#	$3 is the number just "gotten" from the source array and "put" into dest array 

$L2:								#Code for finishing/cleaning up after loop
	move	$2,$0
	j	$31
	.end	main
	.size	main, .-main
	.globl	source
	.data
	.align	2
	.type	source, @object
	.size	source, 28

source:
	.word	3
	.word	1
	.word	4
	.word	1
	.word	5
	.word	9
	.word	0

	.comm	dest,40,4				#Allocates 40 bytes (4 bytes per word/int, 10 ints)
	.ident	"GCC: (GNU) 4.4.1"
