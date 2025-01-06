#include <iostream>
#include <iomanip>
#include "BinomialTreeModel02.h"
#include "Option05.h"
#include <cstdio>

using namespace std;
using namespace fre;
int main(){
    int N=8;
    double S0=106.00,U=1.15125,D=0.86862,R=1.00545;
    double K=100.0;
    double K1=100.00,K2=110.00;
    BinomialTreeModel Model(S0,U,D,R);

    Call call(N,K);
    OptionCalculation callCalculation(&call);
    printf("European Call Option Price: %.2f\n",callCalculation.PricebyCRR(Model));

    Put put(N,K);
    OptionCalculation putCalculation(&put);
    printf("European Put Option Price: %.2f\n",putCalculation.PricebyCRR(Model));

    BullSpread bullspread(N,K1,K2);
    OptionCalculation bullCalculation(&bullspread);
    printf("European Bull Spread Option Price: %.2f\n",bullCalculation.PricebyCRR(Model));

    BearSpread bearspread(N,K1,K2);
    OptionCalculation bearCalculation(&bearspread);
    printf("European Bear Spread Option Price: %.2f\n",bearCalculation.PricebyCRR(Model));
    return 0;
}


//make
// ./OptionPricer05