#pragma once
namespace fre{
    class Function{
        public:
        virtual double Value(double x)=0;
        virtual double Deriv(double x)=0;
        virtual ~Function(){}
    };

    class F1:public Function{
        public:
        double Value(double x);
        double Deriv(double x);
    };

    class F2:public Function{
        private:
        double a;
        public:
        F2(double a_):a(a_){}
        double Value(double x);
        double Deriv(double x);
    };

    class BondFunc:public Function{
        private:
        double c; //yearly coupon
        double f; //face value
        double t; //maturity
        double p; //current price

        public:
        BondFunc(double c_,double f_,double t_,double p_):c(c_),f(f_),t(t_),p(p_){}
        double Value(double x);
        double Deriv(double x);

    };
}