#pragma once
using namespace std;
#include <iostream>
#include <cmath>
#include <iomanip>
namespace fre{
    class BinomialTreeModel{
        private:
        double S0,U,D,R;

        public:
        BinomialTreeModel():S0(0),U(0),D(0),R(0){}
        BinomialTreeModel(double S0_,double U_,double D_,double R_):S0(S0_),U(U_),D(D_),R(R_){}
        ~BinomialTreeModel(){}
        
        double RiskNeutProb() const;
        double CalculateAssetPrice(int n,int i) const;
        void UpdateBinomialTreeModel(double S0_,double U_,double D_,double R_);

        int ValidateInputData() const;
        int GetInputData();

        double GetS0() const {return S0;}
        double GetU() const {return U;}
        double GetD() const {return D;}
        double GetR() const {return R;}
        };

    class BSModel{
        private:double S0,sigma,r;
        public:
        BSModel():S0(0),sigma(0),r(0){}
        BSModel(double S0_,double sigma_,double r_):S0(S0_),sigma(sigma_),r(r_){
            cout << setiosflags(ios::fixed) << setprecision(5);
            cout<<"S0 = "<<S0<<endl;
            cout<<"r = "<<r<<endl;
            cout<<"sigma = "<<sigma<<endl;
        }
        BinomialTreeModel generateBinModel(double T,int N){
            
            cout<<"T = "<<T<<endl<<endl;;
            double h=T/N;
            double U=exp(sigma*sqrt(h));
            double D=1/U;
            double R=exp(r*h);

            cout << setiosflags(ios::fixed) << setprecision(5);
            cout<<"U = "<<U<<endl;
            cout<<"D = "<<D<<endl;
            cout<<"R = "<<R<<endl;
            return BinomialTreeModel(S0,U,D,R);
        }
    };
}