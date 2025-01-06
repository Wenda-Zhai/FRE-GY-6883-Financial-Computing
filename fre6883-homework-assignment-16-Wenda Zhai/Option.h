
#pragma once
#include "BSModel01.h"

class Option
{
    public:
    double T,zl,zu;
    virtual double Payoff(double z)=0; //pure virtual
    virtual double UpperBdCond(BSModel *PtrModel, double t)=0;
    virtual double LowerBdCond(BSModel *PtrModel, double t)=0;

};

class Put: public Option
{
    public:
    double K;
    Put(double K_,double T_, double zl_, double zu_){K=K_,T=T_,zl=zl_,zu=zu_;} // can't use initialization list
    double Payoff(double z);
    double UpperBdCond(BSModel *PtrModel, double t);
    double LowerBdCond(BSModel *PtrModel, double t);
};