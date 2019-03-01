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
    li $t0, 0xF   #counter = 15
                  #default value of RB3 is 0 = decrease
    lui $t1, SFR_BASE_HI

    #sets pin E0-E3 as output
    lw $t2, TRISE($t1)
    andi $t2, $t2, 0xFFF0
    sw $t2, TRISE($t1)
    #sets pin B3 as input
    lw $t3, TRISB($t1)
    andi $t3, $t3, 0x0009
    sw $t3, TRISB($t1)
while:
    lw $t4, PORTB($t1)
    andi $t4, $t4, 0x0008   #to isolate RB3 bit
    sw $t0, LATE($t1)
    li $a0, 500  #2Hz freq
    jal delay
    beq $t4, 0x00, decrease
    #increase
    beq $t0, 0xF, clear1
    addi $t0, $t0, 1
    j while
decrease:
    beq $t0, 0x00, clear2
    sub $t0, $t0, 1
    j while
clear1:
    li $t0, 0x00
    j while
clear2:
    li $t0, 0xF
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
