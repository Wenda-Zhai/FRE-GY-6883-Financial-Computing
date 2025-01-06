#include <iostream>
#include <iomanip>
#include "BinomialTreeModel02.h"
#include "Option05.h"
#include "DoubleDigitOpt.h"
#include "StrangleOpt.h"
#include "ButterflyOpt.h"
#include <cstdio>

using namespace std;
using namespace fre;
int main(){
    int N=8;
    double S0=106.00,U=1.15125,D=0.86862,R=1.00545;
    double K1=100.0,K2=110.0;
    BinomialTreeModel Model(S0,U,D,R);

    DoubleDigitOpt doubleDigitOpt(N,K1,K2);
    OptionCalculation optCalculation_1(&doubleDigitOpt);
    printf("European double digit option price =  %.2f\n",optCalculation_1.PricebyCRR(Model));
    

    StrangleOpt strangleOpt(N,K1,K2);
    OptionCalculation optCalculation_2(&strangleOpt);
    printf("European strangle option price = %.2f\n",optCalculation_2.PricebyCRR(Model));

    ButterflyOpt butterflyOpt(N,K1,K2);
    OptionCalculation optCalculation_3(&butterflyOpt);
    printf("European butterfly option price %.2f\n",optCalculation_3.PricebyCRR(Model));
    
    return 0;
    }