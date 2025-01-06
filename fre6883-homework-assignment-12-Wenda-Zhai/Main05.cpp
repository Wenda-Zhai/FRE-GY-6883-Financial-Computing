#include <iostream>
#include "PathDepOption02.h"
#include <cstdio>
using namespace std;
using namespace fre;
int main()
{
    int d=3;
    Vector S0(d);
    S0[0]=40.0;
    S0[1]=60.0;
    S0[2]=100.0;
    double r=0.03;

    Matrix C(d);
    for(int i=0;i<d;i++) C[i].resize(d);

    C[0][0] =  0.1;  C[0][1] = -0.1;  C[0][2] = 0.0;
    C[1][0] = -0.1;  C[1][1] =  0.2;  C[1][2] = 0.0;
    C[2][0] =  0.0;  C[2][1] =  0.0;  C[2][2] = 0.3;
    MCModel Model(S0,r,C);

   Vector mydelta(d);
   double T=1.0/12.0, K=200.0;
   int m=30;
   ArthmAsianCall Option(T,K,m,mydelta);

   long N=3000;
   double epsilon=0.001;
   cout << "Arithmetic Basket Call Price = " << Option.PriceByMC(Model, N,epsilon,d) << endl;
    for(int i=0;i<d;i++)
    {
        printf("delta[%i] = %f\n",i,Option.GetDelta()[i]);
    }
   return 0;
}

//make
// ./HW12

//paste the result here
/*
Arithmetic Basket Call Price = 2.21242
delta[0] = 0.502694
delta[1] = 0.510783
delta[2] = 0.526854
*/