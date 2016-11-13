        .data
        .word 2, 4, 6, 8
n:      .word 9


        .text
main: 		add     $t0, $0, $zero
		addi    $t1, $zero, 1
		la      $t3, n
		lw      $t3, 0($t3)

fib: 		beq     $t3, $0, finish   # while t != 0
		add     $t2,$t1,$t0	  	# t2 = t1 + t0
		move    $t0, $t1	  	# t0 = t1
		move    $t1, $t2	  	# t1 = t2
		subi    $t3, $t3, 1	  	# t3 -= 1
		j       fib

finish: 	addi    $a0, $t0, 0
		li      $v0, 1		# you will be asked about what the purpose of this line for syscall 
		syscall			
		li      $v0, 10		
		syscall			



# .data tells the assembler the upcoming section is for data (global variables) 
#	consists of a label, the type, and the value (so n is assigned to the integer 9 in this case) 

# .text signals that instructions are about to start 

# Set a breakpoint by making a checkbox and run to it by pressing play

# 34 is the 10th fib number (0, 1, 1, 2, 3, 5, 8, 13, 21, 34)

#n is located at the address 0x 10010010    (to the right of the first address by 16 spaces or 4 words)

#Get 233 when I manually modify the memory location (from n) and 233 when modifiying the actual register at BEQ



