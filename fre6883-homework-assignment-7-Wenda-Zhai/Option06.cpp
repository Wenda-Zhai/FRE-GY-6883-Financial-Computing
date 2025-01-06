#include "Option06.h"
#include "BinomialTreeModel02.h"
#include <iostream>
#include <cmath>
#include <vector>

using namespace std;
namespace fre{
    Option::~Option(){}

    double Call::Payoff(double z) const{
        if (z>K) return z-K;
        return 0.0;
    }

    double Put::Payoff(double z) const{
        if (z<K) return K-z;
        return 0.0;
    }

    double OptionCalculation::PriceByCRR(const BinomialTreeModel &Model)
    {
        double q=Model.RiskNeutProb();
        int N=pOption->GetN();
        vector<double> Price(N+1);
        for(int i=0;i<=N;i++){
            Price[i]=pOption->Payoff(Model.CalculateAssetPrice(N,i));
        }
        for (int n=N-1;n>=0;n--){
            for(int i=0;i<=n;i++){
                Price[i]=(q*Price[i+1]+(1-q)*Price[i])/Model.GetR();
            }
        }
        return Price[0];
    }

    double OptionCalculation::PriceBySnell(const BinomialTreeModel &Model)
    
    {   
        
        double q=Model.RiskNeutProb();
        int N=pOption->GetN();
        vector<double> Price(N+1);
        double ContVal=0;
        for(int i=0;i<=N;i++){
            Price[i]=pOption->Payoff(Model.CalculateAssetPrice(N,i));
        }
        for (int n=N-1;n>=0;n--){
            for(int i=0;i<=n;i++){
                ContVal=(q*Price[i+1]+(1-q)*Price[i])/Model.GetR();
                Price[i]=pOption->Payoff(Model.CalculateAssetPrice(n,i));
                if(ContVal>Price[i])
                {Price[i]=ContVal;}
            }
        }
        return Price[0];
    }
    
}