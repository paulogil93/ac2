#	//Programa 2 - Teste do system call "inkey()"
#	int main(void)
#	{
#		char c;
#		
#		while(1)
#		{
#			while((c = inkey()) == 0);
#			if(c == '\n')
#			break;
#			printStr("Key pressed\n");
#		}
#		return 0;
#	}

		.equ PRINT_STR, 8
		.equ INKEY, 1
		.data
msg:	.asciiz "Key pressed\n"
		.text
		.globl main
main:
iloop:
		li $v0, INKEY
		syscall
		move $t1, $v0
while:
		bne $t1, 0, endw
		beq $t1, '\n', endw
		la $a0, msg
		li $v0, PRINT_STR
		syscall
		j iloop
endw:
		li $v0, 0x00
		jr $ra

