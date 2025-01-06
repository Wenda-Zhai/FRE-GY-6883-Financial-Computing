#include "Option.h"
#include <cmath>

double Put::Payoff(double z)
{
    if(K<z) return 0.0;
    return K-z;
}

//lower bond is 0;
double Put::UpperBdCond(BSModel *PtrModel, double t)
{
    return 0.0;
}

//upper bond is the PV of K
double Put::LowerBdCond(BSModel *PtrModel, double t)
{
    return K*exp(-PtrModel->r * (T-t));
}

