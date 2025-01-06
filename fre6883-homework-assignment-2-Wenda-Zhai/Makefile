CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11

OptionPricer02: OptionPricer02.o BinomialTreeModel.o Option02.o
	$(CC) $(CFLAGS) -o OptionPricer02 OptionPricer02.o BinomialTreeModel.o Option02.o

OptionPricer02.o: OptionPricer02.cpp BinomialTreeModel.h Option02.h
	$(CC) $(CFLAGS) -c OptionPricer02.cpp

Option02.o: BinomialTreeModel.h Option02.h Option02.cpp
	$(CC) $(CFLAGS) -c Option02.cpp
	
BinomialTreeModel.o: BinomialTreeModel.h BinomialTreeModel.cpp
	$(CC) $(CFLAGS) -c BinomialTreeModel.cpp
	
clean:
	rm -rf OptionPricer02 *.o