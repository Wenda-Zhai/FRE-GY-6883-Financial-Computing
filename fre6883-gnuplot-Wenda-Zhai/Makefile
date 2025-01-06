CC = g++
CFLAGS = -Wall -ggdb3 -std=c++11 

plot.exe: gnuplot_linux.o
	$(CC) $(CFLAGS) -o plot.exe gnuplot_linux.o

gnuplot_linux.o: gnuplot_linux.cpp
	$(CC) $(CFLAGS) -c gnuplot_linux.cpp
clean:
	rm -rf plot.exe *.o