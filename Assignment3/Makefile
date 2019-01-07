prog3: thread-main.cpp thread.o boat-monitor.o
	g++ -pthread -o  prog3 thread-main.cpp thread.o boat-monitor.o

thread.o : thread.cpp thread.h
	g++ -pthread -c thread.cpp

boat-monitor.o : boat-monitor.cpp boat-monitor.h
	g++ -pthread -c boat-monitor.cpp

.PHONY : clean
clean :
	-rm -rf thread.o boat-monitor.o prog3