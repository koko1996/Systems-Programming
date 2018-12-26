myshell : lex.yy.o myshell.o
	gcc -o myshell lex.yy.o myshell.o

myshell.o : myshell.c
	gcc -c myshell.c

lex.yy.c : lex.c
	flex lex.c

lex.yy.o : lex.yy.c
	gcc -c lex.yy.c 

.PHONY : clean
clean :
	-rm myshell myshell.o 
