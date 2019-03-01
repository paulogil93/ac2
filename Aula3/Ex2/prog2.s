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
		lui $t1, SFR_BASE_HI

		#sets pin E0 as output
		lw $t2, TRISE($t1)
		andi $t2, $t2, 0xFFFE
		sw $t2, TRISE($t1)
		#sets pin B0 as input
		lw $t3, TRISB($t1)
		andi $t3, $t3, 0x0001
		sw $t3, TRISB($t1)
while:
		lw $t4, PORTB($t1)
		li $t5, -1						#mips not
		xor $t5, $t5, $t4			#xor value with -1
		sw $t5, LATE($t1)
		j while

		jr $ra
