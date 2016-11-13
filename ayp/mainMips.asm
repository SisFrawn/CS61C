.data

.text

main:	
	addi $a1, $0, 50
	addi $a2, $0, 100 
	
	jal addNums
	
	#Print out result of calling addNums
	add $a0, $0, $v1
	li $v0 1
	syscall
	

	#End of program
	li $v0 10
	syscall
	
	
	
	
	
addNums:
	#V1 stores the return value in v1 
	add $v1, $a1, $a2
	jr $ra 