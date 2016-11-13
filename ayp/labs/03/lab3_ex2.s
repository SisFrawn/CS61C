.data
.text 

main:		add $t0 $s0 $0		#t0 = s0
		add $t1 $s1 $0		#t1 = s1
		
		add $t2 $t1 $t0		#t2 = t1 + t0
		add $t3 $t2 $t1		# ...
		add $t4 $t3 $t2		# ...
		add $t5 $t4 $t3		# ...
		add $t6 $t5 $t4		# ...
		add $t7 $t6 $t5		#t7 = t6 + t5
		
		add $a0 $t7 $0		#printing out t7
		li $v0, 1
		syscall
		
		li      $v0, 10		#Exiting		
		syscall 
		