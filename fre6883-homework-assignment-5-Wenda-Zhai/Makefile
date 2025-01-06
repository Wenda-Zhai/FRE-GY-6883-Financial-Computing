CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11

OptionPricer05: OptionPricer05.o BinomialTreeModel02.o Option05.o DoubleDigitOpt.o StrangleOpt.o ButterflyOpt.o
	$(CC) $(CFLAGS) -o OptionPricer05 OptionPricer05.o BinomialTreeModel02.o Option05.o DoubleDigitOpt.o StrangleOpt.o ButterflyOpt.o

OptionPricer05.o: DoubleDigitOpt.h StrangleOpt.h ButterflyOpt.h OptionPricer05.cpp BinomialTreeModel02.h Option05.h
	$(CC) $(CFLAGS) -c OptionPricer05.cpp

Option05.o: BinomialTreeModel02.h Option05.h Option05.cpp
	$(CC) $(CFLAGS) -c Option05.cpp
	
BinomialTreeModel02.o: BinomialTreeModel02.h BinomialTreeModel02.cpp
	$(CC) $(CFLAGS) -c BinomialTreeModel02.cpp

DoubleDigitOpt.o: Option05.h DoubleDigitOpt.h DoubleDigitOpt.cpp
	$(CC) $(CFLAGS) -c DoubleDigitOpt.cpp

StrangleOpt.o: Option05.h StrangleOpt.h StrangleOpt.cpp
	$(CC) $(CFLAGS) -c StrangleOpt.cpp

ButterflyOpt.o: Option05.h ButterflyOpt.h ButterflyOpt.cpp
	$(CC) $(CFLAGS) -c ButterflyOpt.cpp

clean:
	rm -rf OptionPricer05 *.o