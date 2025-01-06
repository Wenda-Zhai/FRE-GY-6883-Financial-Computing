#include "Option02.h"
#include "BinomialTreeModel.h"

#include <iostream>
#include <cmath>
#include <cstdio>

using namespace std;
using namespace fre;

int main(){
    double S0=0,U=0,D=0,R=0;
    if(GetInputData(S0,U,D,R)!=0){
        return -1;
    }
    double K=0.0;
    int N=0;
    printf("EnterCallOptionData\n");
    GetInputData(N,K);
    double *optionPrice=NULL;

    optionPrice=PriceByCRR(S0,U,D,R,N,K,CallPayOff);
    printf("European Call Option Price %.2f\n",optionPrice[0]);
    delete [] optionPrice;

    optionPrice=PriceByCRR(S0,U,D,R,N,K,PutPayOff);
    printf("European Put Option Price %.2f\n",optionPrice[0]);
    delete [] optionPrice;

    optionPrice=PriceByCRR(S0,U,D,R,N,K,DigitCallPayOff);
    printf("European Digit Call Option Price %.2f\n",optionPrice[0]);
    delete [] optionPrice;

    optionPrice=PriceByCRR(S0,U,D,R,N,K,DigitPutPayOff);
    printf("European Digit Put Option Price %.2f\n",optionPrice[0]);
    delete [] optionPrice;
    optionPrice=NULL;// set null once


    return 0;
}