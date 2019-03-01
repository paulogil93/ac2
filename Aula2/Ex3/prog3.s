	.equ READ_CORE_TIMER, 11
	.equ RESET_CORE_TIMER, 12
	.equ PUT_CHAR, 3
	.equ PRINT_INT, 6
	.data
	.text
	.globl main
main:
	li $t0, 500
while:
	lui $t1,0xBF88
	lw $t2,0x6050($t1)
	andi $t2, $t2, 0x000F
	move $a0, $t2
	li $a1, 0x00040002
	li $v0, PRINT_INT
	syscall
	li $a0, '\n'
	li $v0, PUT_CHAR
	syscall
	move $a0, $t0
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