scheduler : scheduler.o funct5.o
	gcc -o scheduler scheduler.o funct5.o

scheduler.o : scheduler.c
	gcc -c scheduler.c

funct5.o : funct5.c
		gcc -c funct5.c


.PHONY : clean
clean :
	-rm scheduler.o funct5.o scheduler
