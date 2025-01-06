CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11

OptionPricer06: OptionPricer06.o BinomialTreeModel02.o Option06.o
	$(CC) $(CFLAGS) -o OptionPricer06 OptionPricer06.o BinomialTreeModel02.o Option06.o 

OptionPricer06.o:  OptionPricer06.cpp BinomialTreeModel02.h Option06.h
	$(CC) $(CFLAGS) -c OptionPricer06.cpp

Option06.o: BinomialTreeModel02.h Option06.h Option06.cpp
	$(CC) $(CFLAGS) -c Option06.cpp
	
BinomialTreeModel02.o: BinomialTreeModel02.h BinomialTreeModel02.cpp
	$(CC) $(CFLAGS) -c BinomialTreeModel02.cpp


clean:
	rm -rf OptionPricer06 *.o