#include <iostream>
#include <cmath>
#include <cstdio>
using namespace std;

class  DefInt{
    private:
    double a, b;
    double (*f)(double x); //remember use (*f) when declare!
    public:
    DefInt(double a1,double b1, double (*f1)(double x1)):a(a1),b(b1),f(f1){
    };
    double ByTrapzoid(unsigned int N) const;
    double BySimpson(unsigned int N) const;

};

//use scope operator to define function outside of class
double DefInt::ByTrapzoid(unsigned int N) const{
    double I=0,x=a,h=(b-a)/N;
    for(int i=0;i<=N;i++){
        if (i==0 || i==N){
            I+=f(x);}
        else{
            I+=2*f(x);}
        x+=h;}
    I*=(b-a)/(2*N);
    return I;
}

//use Simpson Method (midpoint)
double DefInt:: BySimpson(unsigned int N) const{
    double I=0,x=a,h=(b-a)/N;
    x+=(h/2);
    for(int i=1;i<=N;i++){
        I+=f(x);
        x+=h;}
    I*=h;
    return I;
}

double myfun(double x){
    return pow(x,2);
}

int main(){

    double a=1.0,b=2.0;
    int N=200;
    DefInt MyInt(a,b,myfun);
    printf("Integral Result with Trapzoid: %.3f \n",MyInt.ByTrapzoid(N));
    printf("Integral Result (Simpson Method): %.3f\n",MyInt.BySimpson(N));
    return 0;
}

// type g++ HW3.cpp in console
// type ./a.out in console