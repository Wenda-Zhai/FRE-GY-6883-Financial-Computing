#pragma once
#include "BinomialTreeModel02.h"


namespace fre{
    class EurOption
    {
        private:EurOption():N(0){}
        EurOption(const EurOption &option):N(option.N){}
        protected:int N;
        public:EurOption(int N_):N(N_){}
        int GetN() const {return N;}
        virtual double Payoff(double z) const =0; // pure virtual function, must be overriden
        virtual ~EurOption()=0;//pure virtual function
    };
    class Call: public EurOption{
        private:double K;
        public: Call(int N_,double K_):EurOption(N_),K(K_){}
        ~Call(){}
        double Payoff(double z) const;
    };
    class Put: public EurOption{
        private:double K;
        public: Put(int N_,double K_):EurOption(N_),K(K_){}
        ~Put(){}
        double Payoff(double z) const;
    };
    class OptionCalculation{
        private: EurOption *pOption; // dynamic allocation, remember free memory in destructor
        OptionCalculation():pOption(0){}
        OptionCalculation(const OptionCalculation & optioncalculation):pOption(optioncalculation.pOption){}
        public:
        OptionCalculation(EurOption *pOption_):pOption(pOption_){}
        ~OptionCalculation(){}
        double PricebyCRR(const BinomialTreeModel &model);
    };
    
    class BullSpread:public EurOption{
        private:double K1,K2;
        public:BullSpread(int N_,double K1_,double K2_):EurOption(N_),K1(K1_),K2(K2_){}
        ~BullSpread(){}
        double Payoff(double z) const;
    };

    class BearSpread:public EurOption{
        private:double K1,K2;
        public:BearSpread(int N_,double K1_,double K2_):EurOption(N_),K1(K1_),K2(K2_){}
        ~BearSpread(){}
        double Payoff(double z) const;
    };
}