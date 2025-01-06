#include <iostream>
#include "PathDepOption.h"
#include "GmtrAsianCall.h"

using namespace std;
using namespace fre;

int main()
{
    double S0=100.0, r=0.03, sigma=0.2;
    MCModel Model(S0,r,sigma);

    double T =1.0/12.0, K=100.0;
    int m=30;

    ArthmAsianCall Option(T,K,m);
    GmtrAsianCall  CVOption(T,K,m);

    long N=30000;
    double epsilon =0.001;
    Option.PriceByVarRedMC(Model,N,CVOption,epsilon);
    Option.DeltaByVarRedMC(Model,N,CVOption);
    cout << "Arithmetic call price = " << Option.GetPrice() << endl
        << "Error = " << Option.GetPricingError() << endl 
        << "delta = "<< Option.GetDelta()<< endl<<endl;

    Option.PriceByMC(Model,N,epsilon);
    cout << "Price by direct MC = " << Option.GetPrice() << endl
        << "Error = " << Option.GetPricingError() << endl 
        << "delta ="<< Option.GetDelta()<< endl;

    return 0;
}

//make
//./HW11

//paste the result here


/*
Arithmetic call price = 1.42592
Error = 0.000136664
delta = 0.517881

Price by direct MC = 1.44056
Error = 0.0121774
delta =0.526888
*/

