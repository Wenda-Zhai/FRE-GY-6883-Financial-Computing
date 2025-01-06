#include "Option08.h"
#include "BinomialTreeModel02.h"
#include "BinLattice02.h"

#include <iostream>
#include <cmath>

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

    double OptionCalculation::PriceBySnell(
        const BinomialTreeModel & Model,
        BinLattice<double> & PriceTree,
        BinLattice<bool> & StoppingTree)//注意要修改变量 用call by reference
    {double q=Model.RiskNeutProb();
    int N=pOption->GetN();//base class pointer!
    PriceTree.SetN(N);
    StoppingTree.SetN(N);
    double ContVal=0.0;
    for(int i=0;i<=N;i++){
        PriceTree.SetNode(N,i,pOption->Payoff(Model.CalculateAssetPrice(N,i)));
        StoppingTree.SetNode(N,i,1);}
    
    for(int n=N-1;n>=0;n--)
    {for(int i=0;i<=n;i++)
        {ContVal=(q*PriceTree.GetNode(n+1,i+1)+(1-q)*PriceTree.GetNode(n+1,i))/Model.GetR();//注意用[n+1][i+1]减去[n+1][i]
        PriceTree.SetNode(n,i,pOption->Payoff(Model.CalculateAssetPrice(n,i)));
        StoppingTree.SetNode(n,i,1);//默认行权
        if (ContVal>=PriceTree.GetNode(n,i)){//如果售出价值大于行权价值 售出期权(注意是大于等于!)
            PriceTree.SetNode(n,i,ContVal);
            StoppingTree.SetNode(n,i,0);
            }
        }
    }
    return PriceTree.GetNode(0,0);//注意全程只能用GetNode和SetNode 不能用[][]索引
    }

    //PriceByCRR() overloaded function
    double OptionCalculation::PriceByCRR(const BinomialTreeModel&Model,
        BinLattice <double> &PriceTree,
        BinLattice <double> &XTree,
        BinLattice <double> &YTree)
    {   double q=Model.RiskNeutProb();
        int N=pOption->GetN();
        PriceTree.SetN(N);
        XTree.SetN(N);
        YTree.SetN(N);
        double ContVal=0.0;

        for(int i=0;i<=N;i++){
        PriceTree.SetNode(N,i,pOption->Payoff(Model.CalculateAssetPrice(N,i)));}
    
        for(int n=N-1;n>=0;n--)
        {for(int i=0;i<=n;i++)
            {ContVal=(q*PriceTree.GetNode(n+1,i+1)+(1-q)*PriceTree.GetNode(n+1,i))/Model.GetR();
            PriceTree.SetNode(n,i,ContVal);

            //注意要用Model计算S (因为S没有Tree)
            double x_ni=(PriceTree.GetNode(n+1,i+1)-PriceTree.GetNode(n+1,i))/(Model.CalculateAssetPrice(n+1,i+1)-Model.CalculateAssetPrice(n+1,i));
            XTree.SetNode(n,i,x_ni);
            
            double y_ni=(PriceTree.GetNode(n+1,i)-x_ni*Model.CalculateAssetPrice(n+1,i))/Model.GetR();
            YTree.SetNode(n,i,y_ni);
            }
        }

        return PriceTree.GetNode(0,0);
    }
    
}