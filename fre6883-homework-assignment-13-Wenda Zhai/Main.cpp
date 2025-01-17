#include <iostream>
#include <cstdio>
#include "BSModel01.h"
#include "Option.h"
#include "BSEq.h"
#include "ExplicitMethod.h"
using namespace std;

int main()
{
    double S0=100.0,r=0.05,sigma=0.2;
    BSModel Model(S0,r,sigma);

    double K=100.0,T=1.0/12,zl=0.0,zu=2.0*S0; // 1./12. is 1.0/12.0
    Put EuropeanPut(K,T,zl,zu);

    BSEq BSPDE(&Model, &EuropeanPut);

    int imax=3000, jmax=1000;
    ExplicitMethod Method(&BSPDE, imax, jmax);
    Method.SolvePDE();
    
    printf("Price = %.5f\n", Method.v(0.0,S0));
    return 0;
}