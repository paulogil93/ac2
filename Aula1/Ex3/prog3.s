#	//Programa 3 - Teste dos system calls "getChar()" e "putChar()"
#	
#	int main(void)
#	{
#		char c;
#		
#		while(1)
#		{
#			c = getChar();
#			if(c == '\n')
#			break;
#			putChar(c);
#		}
#		return 1;
#	}

		.equ GET_CHAR, 2
		.equ PUT_CHAR, 3
		.text
		.globl main
main:
		li $t0, 0x01
loop:	
		li $v0, GET_CHAR
		syscall
		move $t1, $v0
		beq $t1, '\n', end
		move $a0, $t1
		li $v0, PUT_CHAR
		syscall
		j loop
end:
		li $v0, 0x01
		jr $ra