    .equ READ_CORE_TIMER, 11
    .equ RESET_CORE_TIMER, 12
    .equ PRINT_INT, 6
    .equ SFR_BASE_HI, 0xBF88
    # I/O pin E
    .equ TRISE, 0x6100
    .equ PORTE, 0x6110
    .equ LATE, 0x6120
    # I/O pin B
    .equ TRISB, 0x6040
    .equ PORTB, 0x6050
    .equ LATB, 0x6060
    .data
    .text
    .globl main
main:
    li $t0, 0x0   #counter = 0
    li $t5, 0x0   #msb = most significant bit
    lui $t1, SFR_BASE_HI

    #sets pin E0-E3 as output
    lw $t2, TRISE($t1)
    andi $t2, $t2, 0xFFF0
    sw $t2, TRISE($t1)
    #sets pin B2 as input
    lw $t3, PORTB($t1)
    andi $t3, $t3, 0x0004
    sw $t3, PORTB($t1)
while:
    sw $t0, LATE($t1)
    lw $t4, PORTB($t1)
    andi $t4, $t4, 0x0004
    li $a0, 666  #1.5Hz freq => number of the beast \m/
    jal delay
    beq $t4, 0x00, left
    #right
    move $t5, $t0
    sll $t5, $t5, 3
    li $t6, -1
    xor $t6, $t6, $t5
    andi $t6, $t6, 0x0008 #not lsb
    srl $t0, $t0, 1
    xor $t0, $t0, $t6
    j while
left:
    move $t5, $t0
    srl $t5, $t5, 3
    li $t6, -1
    xor $t6, $t6, $t5
    andi $t6, $t6, 0x0001 #not msb
    sll $t0, $t0, 1
    xor $t0, $t0, $t6
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
