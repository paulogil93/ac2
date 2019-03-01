Add these lines to .bashrc :)

sudo nano ~/.bashrc

paste, CTRL+X to close, Y to save


-------------------------------------------------------------------------------------------------
# Alias to compile, flash detpic32 and open pterm in one run

# It compiles *.c or *.s files

# pic32c -> compiles and flashes detpic32

# pic32r -> compiles, flashes and opens pterm

alias pic32c='_pic32c(){ (pcompile $1.c || pcompile $1.s) && ldpic32 $1.hex; };_pic32c'

alias pic32r='_pic32r(){ (pcompile $1.c || pcompile $1.s) && ldpic32 $1.hex && pterm; };_pic32r'
-------------------------------------------------------------------------------------------------

Recomendo o uso do editor Visual Studio Code com Intellisense ativado(code-completion).




