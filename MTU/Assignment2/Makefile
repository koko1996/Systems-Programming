prog2 : prog2.o helper.o
	gcc -pthread -o  prog2 prog2.o helper.o

prog2.o : prog2.c prog2.h
	gcc -c prog2.c

helper.o : helper.c prog2.h
	gcc -c helper.c

.PHONY : clean
clean :
	-rm helper.o prog2.o prog2 
