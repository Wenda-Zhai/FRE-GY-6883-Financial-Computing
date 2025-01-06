// TestEuropeanOption.cpp
// Test program for the exact solutions of European options.
// (C) Datasim Component Technology BV 2003-2006

#include "EuropeanOption.hpp"
#include <iostream>
#include <cstdio>
#include <cmath>

int main()
{
	// Call option on a stock
	EuropeanOption callOption_1;
	printf("Q1: %s option on a stock: %.5f\n",callOption_1.GetOptType(),callOption_1.Price());

	// Put option on a stock index
	EuropeanOption putOption_1(0.10,0.30,50.0,3.0/12.0,50.0,0.10);
	putOption_1.toggle();
	printf("Q2: %s option on a stock: %.5f\n",putOption_1.GetOptType(),putOption_1.Price());

	//Put Option with dividend
	double dividend=1.5;
	double t_dividend= 2.0/12.0; //not 2/12, because 2/12=0!
	double S0_prime= 50.0 - dividend * exp(-0.10*t_dividend);
	EuropeanOption putOption_2(0.10,0.30,50.0,3.0/12.0,S0_prime,0.10);
	putOption_2.toggle();
	printf("Q3: %s option with dividend: %.5f\n",putOption_2.GetOptType(),putOption_2.Price());
	return 0;
}

// make
// ./TestEuropeanOption

//paste the result here
/*
Q1: Call option on a stock: 2.13293
Q2: Put option on a stock: 2.37561
Q3: Put option with dividend: 3.03039
*/

