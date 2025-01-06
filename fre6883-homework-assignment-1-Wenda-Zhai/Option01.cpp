#include "Option01.h"
#include "BinomialTreeModel.h"
#include <iostream>
#include <cmath>
using namespace std;

namespace fre {
    double PriceByCRR(double S0, double U, double D, double R, unsigned int N, double K)
    {
        double q = RiskNeutProb(U, D, R);
        double Price[N+1];
        for (unsigned int i = 0; i < sizeof(Price) / sizeof(Price[0]); i++)
            {Price[i] = 0.0;}

        for (unsigned int i = 0; i <= N; i++)
        {
            Price[i] = CallPayoff(CalculateAssetPrice(S0, U, D, N, i), K);
        }
        double call_price=0;
        for (unsigned int i=0;i<=N;i++)
        {
            call_price+=factorial(N)/(factorial(i)*(factorial(N-i)))*pow(q,i)*pow(1-q,N-i)*Price[i];

        }
        call_price/=pow(R,N);
        // call_price*=pow(R,-N); //->inf
        return  call_price;
    }

    double CallPayoff(double z, double K)
    {
        if (z > K) return z - K;
        return 0.0;
    }
    
    int factorial(unsigned int x)
    {
        int result=1;
        for(int i=x;i>=1;i--)
        {result*=i;}
        return result;
    }
}
