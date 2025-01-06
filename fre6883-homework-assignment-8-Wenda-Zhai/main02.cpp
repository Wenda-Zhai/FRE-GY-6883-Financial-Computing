#include <iostream>
#include <iomanip>
#include <cstdio>
#include <cmath>
#include "Function02.h"
#include "NonlinearSolver02.h"
using namespace std;
using namespace fre;

int main(){

    double F=100.0, T=3.0, C=1.2, P=98.56,Acc=0.0001;
    double LEnd=0.0, REnd=1.0, Guess=0.2;
    NonlinearSolver solver(P,LEnd,REnd,Acc,Guess);

    BondFunc MyBond(C,F,T,P);
    printf("Yield by Bisection Method: %.4f\n",solver.SolveByBisect(&MyBond));
    printf("Yield by Newton Raphson Method: %.4f\n",solver.SolveByNR(&MyBond));

    return 0;
}


//paste the result here

/*
Yield by Bisection Method: 0.0168
Yield by Newton Raphson Method: 0.0168
*/