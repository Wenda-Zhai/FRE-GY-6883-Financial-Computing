#include "StrangleOpt.h"
namespace fre{
    double StrangleOpt::Payoff(double z) const{
        if(z<=K1)
        {return K1-z;}
        if(z>K2)
        {return z-K2;}
        return 0.0;
    }
}