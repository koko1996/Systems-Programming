prog3: thread-main.cpp thread.o
	gcc -pthread -o  prog3 thread-main.cpp thread.o

thread.o : thread.cpp thread.h
	gcc -c thread.cpp

.PHONY : clean
clean :
	-rm thread.o prog3