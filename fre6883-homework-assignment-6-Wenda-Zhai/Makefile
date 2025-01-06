CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11

OptionPricer08: OptionPricer08.o BinomialTreeModel02.o Option08.o BinLattice02.o
	$(CC) $(CFLAGS) -o OptionPricer08 OptionPricer08.o BinomialTreeModel02.o Option08.o 

OptionPricer08.o:  OptionPricer08.cpp BinomialTreeModel02.h Option08.h
	$(CC) $(CFLAGS) -c OptionPricer08.cpp

Option08.o: BinomialTreeModel02.h Option08.h Option08.cpp BinLattice02.h
	$(CC) $(CFLAGS) -c Option08.cpp
	
BinomialTreeModel02.o: BinomialTreeModel02.h BinomialTreeModel02.cpp
	$(CC) $(CFLAGS) -c BinomialTreeModel02.cpp

clean:
	rm -rf OptionPricer08 *.o *.gch