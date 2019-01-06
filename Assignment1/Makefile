prog1 : prog1.o
	gcc -pthread -o  prog1 prog1.o

prog1.o : prog1.c prog1.h
	gcc -c prog1.c

.PHONY : clean
clean :
	-rm prog1 prog1.o
