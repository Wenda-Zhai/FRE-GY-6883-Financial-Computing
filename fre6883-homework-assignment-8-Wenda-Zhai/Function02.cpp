#include "Function02.h"
#include <cmath>
#include <cstdio>
namespace fre{
    double F1::Value(double x){return x*x-2;}
    double F1::Deriv(double x){return 2*x;}
    double F2::Value(double x){return x*x-a;}
    double F2::Deriv(double x){return 2*x;}

    double BondFunc::Value(double x){
        double price=0;
        for (int i=1;i<=t;i++)
        {price+=c*exp(-x*i);}
        price+=f*exp(-x*t);
        return price;
    }

    double BondFunc::Deriv(double x){
        double Modified_duration=0;
        for(int i=1;i<=t;i++)
        {Modified_duration+=-i*c*exp(-x*i);}
        Modified_duration+=-t*f*exp(-x*t);
        return Modified_duration;
    }


}