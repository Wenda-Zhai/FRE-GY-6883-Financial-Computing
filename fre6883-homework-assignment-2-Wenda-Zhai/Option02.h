#pragma once
namespace fre{
    //inputting and displaying option data
    int GetInputData(int &N,double &K);

    //pricing European Option
    double *PriceByCRR(double S0, double U,double D,double R, int N,double K,double (*Payoff)(double z,double K));

    double CallPayOff(double z,double K);
    double PutPayOff(double z, double K);
    double DigitCallPayOff(double z, double K);
    double DigitPutPayOff(double z, double K);
}