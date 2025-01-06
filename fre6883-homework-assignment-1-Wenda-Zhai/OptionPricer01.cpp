// OptionPricer01.cpp
//main function
//包括主函数

#include "BinomialTreeModel.h" //what if not include this?
#include "Option01.h"
#include <iostream>
#include <iomanip>
#include <cmath>

using namespace std;
using namespace fre;

int main()
{
    double u = 1.15125, d = 0.86862, r = 1.00545;
    double s0 = 106.00;
    double k = 100.00;
    const unsigned int N = 8; //unsigned not negative -N

    double optionPrice = PriceByCRR(s0, u, d, r, N, k);
    cout << "European call option price = " << fixed << setprecision(2) << optionPrice << endl;
    // cout<<pow(r,-N)<<' '<<r<<' '<<N<<endl; //N is unsigned, cause error to be inf
    return 0;
}
/*
European call option price = 21.68
*/
