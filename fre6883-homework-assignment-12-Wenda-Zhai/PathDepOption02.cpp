#include "PathDepOption02.h"
#include <cmath>


namespace fre {

    void Rescale(SamplePath& S, double x, int j)
    //注意这里有变化 只rescale第j个股票
    {
        int m = S.si ze();
        for (int i = 0; i < m; i++) 
        {
            S[i][j] = x * S[i][j];
        }
    }


    double PathDepOption::PriceByMC(MCModel& Model, long N,double epsilon, int d)
    {
        double H = 0.0;
        Vector Hsq(d),Heps(d); //这里不一样 有d个sqaure和d个error
        SamplePath S(m);

        for (long i = 0; i < N; i++)
        {
            Model.GenerateSamplePath(T, m, S);
            H = (i * H + Payoff(S)) / (i + 1.0);
            
            for (long j = 0; j < d; j++) //遍历d个股票
            {
                Hsq[j]=(i*Hsq[j]+pow(Payoff(S),2.0))/(i+1.0);
                Rescale(S,1.0+epsilon,j);
                Heps[j]=(i*Heps[j]+Payoff(S))/(i+1.0);
                Rescale(S,1.0/(1.0+epsilon),j); //注意rescale回去 (求偏导的时候其他价格不能变)
            }
        }
        Price = std::exp(-Model.GetR() * T) * H; //注意这里是std的exp贴现
        for(long j=0;j<d;j++)
        {
            delta[j]=std::exp(-Model.GetR()*T)*(Heps[j]-H)/(Model.GetS0()[j]*epsilon); //注意这里是std的exp贴现
        }
        return Price;
    }

    double ArthmAsianCall::Payoff(const SamplePath& S) const
    {
        double Ave = 0.0;
        int d = S[0].size(); //注意是S[0]的size设置为d
        Vector one(d); //把one设置为d
        for (int i = 0; i < d; i++)
        {
            one[i] = 1.0;
        }

        for (int k = 0; k < m; k++)
        {
            Ave = (k * Ave + (one ^ S[k])) / (k + 1.0); //用scaler operator
        }

        if (Ave < K) return 0.0;
        return Ave - K;
    }
}
