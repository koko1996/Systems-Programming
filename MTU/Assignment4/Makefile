main : main.o compute
	gcc -pthread -o  main main.o

main.o : main.c
	gcc -c main.c

compute : compute.o 
	gcc -pthread -o  compute compute.o

compute.o : compute.c
	gcc -c compute.c

.PHONY : clean
clean :
	-rm -f main main.o compute compute.o
