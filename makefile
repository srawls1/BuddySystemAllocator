# makefile

CXXFLAGS = -std=c++11 -Wall -g

CXXFLAGS_TESTING = -std=c++11 -Wall -O2

UNAME_S := $(shell uname -s)

ifeq ($(UNAME_S),Darwin)
	CXX = clang++
else
	CXX = g++
endif

all: memtest

testing: memtest_f

ackerman.o: ackerman.cpp ackerman.h
	$(CXX) $(CXXFLAGS) -c ackerman.cpp

my_allocator.o : my_allocator.cpp my_allocator.h
	$(CXX) $(CXXFLAGS) -c my_allocator.cpp

memtest.o : memtest.cpp
	$(CXX) $(CXXFLAGS) -c memtest.cpp

memtest: memtest.o ackerman.o my_allocator.o
	$(CXX) $(CXXFLAGS) -o memtest memtest.o ackerman.o my_allocator.o

ackerman_f.o: ackerman.cpp ackerman.h
	$(CXX) $(CXXFLAGS_TESTING) -c $<

my_allocator_f.o : my_allocator.cpp my_allocator.h
	$(CXX) $(CXXFLAGS_TESTING) -c $<

memtest_f.o : memtest.cpp
	$(CXX) $(CXXFLAGS_TESTING) -c $<

memtest_f: memtest_f.o ackerman_f.o my_allocator_f.o
	$(CXX) $(CXXFLAGS_TESTING) -o memtest memtest.o ackerman.o my_allocator.o

clean:
	rm *.o memtest
