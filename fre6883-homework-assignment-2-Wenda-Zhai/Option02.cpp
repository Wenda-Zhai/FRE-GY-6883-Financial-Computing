#include "Option02.h"
#include "BinomialTreeModel.h" //include functions from BinomialTreeModel.cpp?
#include <iostream>
#include <cmath>
#include <cstdio>
using namespace std;

namespace fre{
    //inputting and displaying option data
    int GetInputData(int &N,double &K)
    {
        printf("Enter steps to expiration N:\n");
        cin>>N;
        printf("Enter strike price K:\n");
        cin>>K;
        return 0;
    }

    //pricing European Option
    double *PriceByCRR(double S0, double U,double D,double R, int N,double K,double (*Payoff)(double z,double K))
    {
        double q=RiskNeutProb(U,D,R);
        double *Price=new double[N+1]; //dynamic allocation (why? because we don't know the length)
        for(int i=0;i<=N;i++){
            Price[i]=Payoff(CalculateAssetPrice(S0,U,D,N,i),K);
        }
        for(int n=N-1;n>=0;n--){
            for(int i=0;i<=N;i++){
                Price[i]=(q*Price[i+1]+(1-q)*Price[i])/R;
            }
        }
        return Price;
    }

    double CallPayOff(double z,double K)
    {
        if(z>=K)
        {return z-K;}
    return 0.0; //can we return 0 (default int?)
    }
    double PutPayOff(double z, double K)
    {
        if(K>=z)
        {return K-z;}
    return 0.0;
    }
    double DigitCallPayOff(double z, double K)
    {
        if(z>=K)
        {return 1.0;}
    return 0.0;
    }
    double DigitPutPayOff(double z, double K)
    {
        if(K>=z)
        {return 1.0;}
    return 0.0;
    }
}