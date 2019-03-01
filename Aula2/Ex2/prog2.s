#void delay(int ms) 
#{ 
#   for(; ms > 0; ms--) 
#   { 
#      resetCoreTimer(); 
#      while(readCoreTimer() < K); 
#   } 
#}

		.equ READ_CORE_TIMER, 11
		.equ RESET_CORE_TIMER, 12
		.equ PRINT_INT, 6
		.equ PUT_CHAR, 3 
		.data
		.text
		.globl main
main:
		li $t0, 0x00
		li $t1, 2000
while:
		addi $t0, $t0, 1
		move $a0, $t0
		li $a1, 10
		li $v0, PRINT_INT
		syscall
		li $a0, ' '
		li $v0, PUT_CHAR
		syscall
		move $a0, $t1
		jal delay
		j while
		jr $ra

# delay(int ms) function
delay:
	subu $sp, $sp, 4
	sw $ra, 0($sp)
	move $s0, $a0
for:	
	ble $s0, 0x00, endf
	li $v0, RESET_CORE_TIMER
	syscall
read:
	li $v0, READ_CORE_TIMER
	syscall
	blt $v0, 20000, read
	sub $s0, $s0, 1
	j for
endf:
	lw $ra, 0($sp)
	addiu $sp, $sp, 4
	jr $ra