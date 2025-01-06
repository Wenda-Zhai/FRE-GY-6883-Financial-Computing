#include "BinomialTreeModel02.h"
#include <iostream>
#include <cmath>
using namespace std;

namespace fre{
    double BinomialTreeModel::RiskNeutProb() const{
        return (R-D)/(U-D);
    }
    double BinomialTreeModel::CalculateAssetPrice(int n,int i) const{
        return S0*pow(U,i)*pow(D,n-i);
    }
    void BinomialTreeModel::UpdateBinomialTreeModel(double S0_,double U_,double D_,double R_){
        S0=S0_;U=U_;D=D_;R=R_;
    }
    int BinomialTreeModel::GetInputData(){
        cout<<"Entering S0:";cin>>S0;
        cout<<"Entering U:";cin>>U;
        cout<<"Entering D:";cin>>D;
        cout<<"Entering R:";cin>>R;
        cout<<endl;
        
        if(S0<=0.0||U<=-1.0||D<=-1.0||U<=D||R<=-1.0)
        {cout<<"Illegal data ranges"<<endl;
        cout<<"Terminating the program"<<endl;
        return -1;}

        if (R>=U||R<=D)
        {cout<<"Arbitrage Exists"<<endl;
        cout<<"Terminating the program"<<endl;
        return -1;}

        cout<<"Input data checked"<<endl;
        cout<<"There is no arbitrage"<<endl<<endl;
        return 0;
    }
    
    int BinomialTreeModel::ValidateInputData() const{

        if(S0<=0.0||U<=0.0||D<=0.0||U<=D||R<=0.0)
        {cout<<"Illegal data ranges"<<endl;
        cout<<"Terminating the program"<<endl;
        return -1;}

        if(R>=U || U<=D)
        {cout<<"Arbitrage Exists"<<endl;
        cout<<"Terminating the program"<<endl;
        return -1;}

        cout<<"Input data checked"<<endl;
        cout<<"There is no arbitrage"<<endl<<endl;
        return 0;
    }
}