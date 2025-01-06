#include "ButterflyOpt.h"
namespace fre{
    double ButterflyOpt::Payoff(double z) const{
        if(z>K1 && z<=(K1+K2)/2)
        {return z-K1;}
        if(z>(K1+K2)/2 && z<=K2)
        {return K2-z;}
        return 0.0;
    }
}