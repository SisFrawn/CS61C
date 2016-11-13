.data
shouldben1:	.asciiz "Should be -1, and firstposshift and firstposmask returned: "
shouldbe0:	.asciiz "Should be 0 , and firstposshift and firstposmask returned: "
shouldbe16:	.asciiz "Should be 16, and firstposshift and firstposmask returned: "
shouldbe31:	.asciiz "Should be 31, and firstposshift and firstposmask returned: "

.text
main:
	la	$a0, shouldbe31
	jal	print_str
	lui	$a0, 0x8000	# should be 31
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x8000
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe16
	jal	print_str
	lui	$a0, 0x0001	# should be 16
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	lui	$a0, 0x0001
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldbe0
	jal	print_str
	li	$a0, 1		# should be 0
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	li	$a0, 1
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	la	$a0, shouldben1
	jal	print_str
	move	$a0, $0		# should be -1
	jal	first1posshift
	move	$a0, $v0
	jal	print_int
	jal	print_space

	move	$a0, $0
	jal	first1posmask
	move	$a0, $v0
	jal	print_int
	jal	print_newline

	li	$v0, 10
	syscall

first1posshift:
	### YOUR CODE HERE ###
	beqz $a0, argWasZero			#if arg == 0, return -1
	
	add $t0, $0, $0				# count = 0
	Loop:	beq $t0, 32, argWasZero		# while(count < 31)
		
		andi $t1, $a0, 0x80000000		#if sign bit & arg is not zero... the bit is on, return 31 - count 
		bne $t1, 0, foundOneBit
		
		sll $a0, $a0, 1				# arg << 1
		
		addi $t0, $t0, 1			# count++
		j Loop			

	argWasZero:
		addi $v0, $0, -1
		j exit
		
	foundOneBit:
		#If the one bit was found, 31 - count will be the position I should place in $v0
		addi $v0, $0, 31
		sub $v0, $v0, $t0
		j exit  
		
	exit: 
		jr $ra 

first1posmask:
	### YOUR CODE HERE ###
	beqz $a0, argWasZero2			#if arg == 0, return -1
	
	
	add $t1, $0, 0x80000000			# bitmask = 0x80000000 (only sign bit on)
	add $t0, $0, $0				# count = 0
	Loop2:	beq $t0, 32, argWasZero2	# while(count < 31)
		
		
		and $t2, $a0, $t1		#if bitmask & arg is not zero... the bit is on, return 31 - count 
		bne $t2, 0, foundOneBit2
		
		srl $t1, $t1, 1				# bitmask >> 1
		
		addi $t0, $t0, 1			# count++
		j Loop2			

	argWasZero2:
		addi $v0, $0, -1
		j exit2
		
	foundOneBit2:
		#If the one bit was found, 31 - count will be the position I should place in $v0
		addi $v0, $0, 31
		sub $v0, $v0, $t0
		j exit2 
		
	exit2: 
		jr $ra 

print_int:
	move	$a0, $v0
	li	$v0, 1
	syscall
	jr	$ra

print_str:
	li	$v0, 4
	syscall
	jr	$ra

print_space:
	li	$a0, ' '
	li	$v0, 11
	syscall
	jr	$ra

print_newline:
	li	$a0, '\n'
	li	$v0, 11
	syscall
	jr	$ra
