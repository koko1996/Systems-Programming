CXX=g++
WARNINGS= -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused  
OPTIMIZATION= -O2
CXXFLAGS= -g -std=c++11
RM=rm -f
NODEBUG=-DNDEBUG
PTHREAD=-lpthread 

.PHONY: all clean

all: mapreduce

mapreduce: main.cpp helper.o buffer.hpp
	$(CXX) $(CXXFLAGS) -o mapreduce main.cpp helper.o  $(PTHREAD)

helper.o: helper.cpp helper.hpp
	$(CXX) $(CXXFLAGS) -c helper.cpp 

	
clean: 
	$(RM) mapreduce helper.o buffer.o
