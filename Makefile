all: program main operacoes_arquivo gerador_arquivos utilitarios arvore_b_estrela

program: main.o operacoes_arquivo.o operacoes_arquivo.o gerador_arquivos.o utilitarios.o arvore_b_estrela.o
	gcc -o program main.o operacoes_arquivo.o gerador_arquivos.o utilitarios.o arvore_b_estrela.o -Wall -lm

main: main.o 
	gcc -g -c main.c
	
	
operacoes_arquivo: operacoes_arquivo.c operacoes_arquivo.h
	gcc -g -c operacoes_arquivo.c


gerador_arquivos: gerador_arquivos.c gerador_arquivos.h
	gcc -g -c gerador_arquivos.c
	
	
utilitarios: utilitarios.c utilitarios.h
	gcc -g -c utilitarios.c
	
arvore_b_estrela: arvore_b_estrela.c arvore_b_estrela.h
	gcc -g -c arvore_b_estrela.c
	
clean:
	rm *.o
run:
	./program
