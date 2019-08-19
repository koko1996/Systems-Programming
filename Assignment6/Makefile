CXX=gcc
WARNINGS= -Wall -Wextra -Wcast-align -Wcast-qual -Wctor-dtor-privacy -Wdisabled-optimization -Wformat=2 -Winit-self -Wlogical-op -Wmissing-declarations -Wmissing-include-dirs -Wnoexcept -Woverloaded-virtual -Wredundant-decls -Wsign-conversion -Wsign-promo -Wstrict-null-sentinel -Wstrict-overflow=5 -Wswitch-default -Wundef -Wno-unused  
OPTIMIZATION= -O2
CXXFLAGS= -g 
RM=rm -f
NODEBUG=-DNDEBUG
PTHREAD=-lpthread 

.PHONY: all clean

all: closh

closh: closh.c
	$(CXX) $(CXXFLAGS) -o closh closh.c 
	
clean: 
	$(RM) closh 
