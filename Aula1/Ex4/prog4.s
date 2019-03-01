#	//Programa 4 - teste dos system calls de leitura
#	//e impressão de inteiros
#
#	void main(void)
#	{
#		int value;
#
#		while(1)
#		{
#			printStr("\nIntroduza um numero(sinal e modulo): ");
#			value = readInt10();
#			printStr("\nValor lido em base 2: ");
#			printInt(value, 2);
#			printStr("\nValor lido em base 16: ");
#			printInt(value, 16);
#			printStr("\nValor lido em base 10(unsigned): ");
#			printInt(value, 10);
#			printStr("\nValor lido em base 10(signed): ");
#			printInt(value);
#		}
#	}

		.equ PRINT_STR, 8
		.equ PRINT_INT, 6
		.equ PRINT_INT10, 7
		.equ READ_INT10, 5
		.data
str1:	.asciiz "\nIntroduza um numero(sinal e modulo): "
str2:	.asciiz "\nValor lido em base 2: "
str3:	.asciiz "\nValor lido em base 16: "
str4:	.asciiz "\nValor lido em base 10(unsigned): "
str5:	.asciiz "\nValor lido em base 10(signed): "
str6:	.asciiz "\n"
		.text
		.globl main
main:
		li $t0, 0x01
loop:
		bne $t0, 0x01, end
		la $a0, str1
		li $v0, PRINT_STR
		syscall
		li $v0, READ_INT10
		syscall
		move $t1, $v0
		la $a0, str2
		li $v0, PRINT_STR
		syscall
		move $a0, $t1
		li $a1, 0x02
		li $v0, PRINT_INT
		syscall
		la $a0, str3
		li $v0, PRINT_STR
		syscall
		move $a0, $t1
		li $a1, 16
		li $v0, PRINT_INT
		syscall
		la $a0, str4
		li $v0, PRINT_STR
		syscall
		move $a0, $t1
		li $a1, 0x0A
		li $v0, PRINT_INT
		syscall
		la $a0, str5
		li $v0, PRINT_STR
		syscall
		move $a0, $t1
		li $v0, PRINT_INT10
		syscall
		la $a0, str6
		li $v0, PRINT_STR
		syscall
		j loop
end:
		jr $ra
