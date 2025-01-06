#include "BinomialTreeModel02.h"
#include "Option06.h"
#include <iostream>
#include <iomanip>
using namespace std;
using namespace fre;

int main(){

double S0=106.0;
double r=0.058;
double sigma=0.46;
double T=(double)9/12;
double K=100.0;
int N=8;

BSModel myBSModel(S0,sigma,r);
Call call(N,K);
BinomialTreeModel Model=myBSModel.generateBinModel(T,N);
OptionCalculation callCalculation(&call);

cout<<"American call option price: "<<fixed<<setprecision(3)<<callCalculation.PriceBySnell(Model)<<endl;
return 0;
}


//paste the result here
/*
S0 = 106.00000
r = 0.05800
sigma = 0.46000
K = 100.00000
N = 8
T = 0.75000

U = 1.15125
D = 0.86862
R = 1.00545
American call option price: 21.682
*/
