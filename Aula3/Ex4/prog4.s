    .equ SFR_BASE_HI, 0xBF88
    .equ READ_CORE_TIMER, 11
    .equ RESET_CORE_TIMER, 12
    # I/O pin E
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    .data
    .text
    .globl main
main:
    li $t0, 0x00  #v
    lui $t1, SFR_BASE_HI

    #sets pin E0 as output
    lw $t2, TRISE($t1)
    andi $t2, $t2, 0xFFFE
    sw $t2, TRISE($t1)
while:
    sw $t0, LATE($t1)
    li $a0, 500   #f=1/T => f=1/0.5 => f=2Hz
    jal delay
    xor $t0, $t0, 1
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
