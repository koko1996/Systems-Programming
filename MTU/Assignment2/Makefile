prog2 : prog2.o
	gcc -pthread -o  prog2 prog2.o

prog2.o : prog2.c
	gcc -c prog2.c

.PHONY : clean
clean :
	-rm prog2 prog2.o
