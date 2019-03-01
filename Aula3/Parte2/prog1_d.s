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
    li $t3, 0x0   #msb = most significant bit
    lui $t1, SFR_BASE_HI

    #sets pin E0-E3 as output
    lw $t2, TRISE($t1)
    andi $t2, $t2, 0xFFF0
    sw $t2, TRISE($t1)
while:
    sw $t0, LATE($t1)
    li $a0, 666  #1.5Hz freq => number of the beast \m/
    jal delay
    move $t3, $t0
    srl $t3, $t3, 3
    li $t4, -1
    xor $t4, $t4, $t3
    andi $t4, $t4, 0x0001 #not msb
    sll $t0, $t0, 1
    xor $t0, $t0, $t4
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
