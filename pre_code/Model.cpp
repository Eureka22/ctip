#include "Model.h"
#include "Util.h"

#include <cstdio>
#include <cmath>

#define eps 1e-10

using namespace std;

Model::Model()
{
}

int     Model::SampleCore(vector<Post*> postList)
{
    int startTime = -1;
    coreSet.clear();
    coreMap.clear();
    coreList.clear();
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        Post* post = postList[i];
        if (startTime == -1 || startTime > post -> postTime)
            startTime = post -> postTime;
    }
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        Post* post = postList[i];
        int t = post -> postTime - startTime;
        //if (t <= 10 && t >= 5)
        if (t <= 5)
        {
            User* user = post -> user;
            int uid = user -> id;
            if (coreSet.count(uid) == 0)
            {
                int cid = (int) coreList.size();
                coreSet.insert(uid);
                coreList.push_back(uid);
                coreMap.insert(make_pair(uid, cid));
            }
            for (unsigned int j = 0; j < user -> followerList.size(); j ++)
            {
                int fid = user -> followerList[j] -> id;
                if (coreSet.count(fid) == 0)
                {
                    coreSet.insert(fid);
                    coreList.push_back(fid);
                }
            }
        }
    }
    printf("Sampled %d core users.\n", (int) coreList.size());
    return 0;
}

int     Model::Load(vector<Post*> postList, string fileDir)
{
    N.clear();
    A.clear();
    B.clear();
    int startTime = -1;
    maxT = 0;
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        Post* post = postList[i];
        if (post -> postTime < startTime || startTime == -1)
            startTime = post -> postTime;
        if (post -> postTime > maxT)
            maxT = post -> postTime;
    }
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        Post* post = postList[i];
        int t = post -> postTime - startTime;
        for (unsigned int j = N.size(); (int) j <= t; j ++)
        {
            N.push_back(0);
            A.push_back(0);
            B.push_back(0);
        }
        N[t] ++;
        if (post -> user == NULL)
        {
            printf("Error!\n");
            continue;
        }
        int uid = post -> user -> id;
        if (coreSet.count(uid) == 0)
            continue;   
        A[t] ++;
    }
    for (unsigned int t = 0; t < N.size(); t ++)
    {
        B[t] = coreSet.size() - A[t];
    }
    FILE* fout = fopen(fileDir.c_str(), "w");
    for (unsigned int i = 0; i < N.size(); i ++)
    {
        fprintf(fout, "%d %d\n", i, N[i]);
    }
    fclose(fout);
    minT = startTime;
    printf("Loaded.\n");
    return 0;
}

int     Model::Load(DataLoader* dataLoader)
{
    for (unsigned int i = 0; i < dataLoader -> sourceList.size(); i ++)
    {
        int sid = dataLoader -> sourceList[i];
        vector<Post*> postList;
        for (unsigned int j = 0; j < dataLoader -> postList.size(); j ++)
        {
            if (dataLoader -> postList[j] -> sourcePost -> id == sid)
            {
                postList.push_back(dataLoader -> postList[j]);
            }
        }
        SampleCore(postList);
        /*
        Load(postList, "exp/n" + Util::Int2Str(i + 1));
        LearnFactor("exp/factor_hotpost_" + Util::Int2Str(i + 1));
        PredGroup("exp/pred_hotpost_" + Util::Int2Str(i + 1));
        */
        PredMicro(dataLoader -> postList);
    }
    return 0;
}

int     Model::LearnFactor(string fileDir)
{
    U.clear();
    D.clear();
    vector<int> num;
    for (unsigned int t = 0; t < N.size(); t ++)
    {
        U.push_back(0);
        D.push_back(0);
        num.push_back(0);
    }
    FILE* fout = fopen(fileDir.c_str(), "w");
    double sumU = 0, sumD = 0;
    for (unsigned int t = 0; t < N.size() - 1; t ++)
    {
        for (unsigned int h = t + 1; h < N.size() - 1; h ++)  
        {
            if (h == t)
                continue;
            double u = 0, d = 0;
            double tmp = (log(N[t + 1]) - log(N[t]) + 0.0) / (B[t] + eps);
            tmp += (log(N[h]) - log(N[h + 1]) + 0.0) / (B[h] + eps);
            tmp /= (A[t] + 0.0) / B[t] - (A[h] + 0.0) / B[h] + eps;
            u = exp(tmp);

            tmp = (log(N[t + 1]) - log(N[t]) + 0.0) / (A[t] + eps);
            tmp += (log(N[h]) - log(N[h + 1]) + 0.0) / (A[h] + eps);
            tmp /= (B[t] + 0.0) / A[t] - (B[h] + 0.0) / A[h] + eps;
            d = exp(tmp);
            //printf("%.5lf %.5lf\n", u, d);
            //fprintf(fout, "%.5lf %.5lf\n", u, d);
            if (u > 0 && u < 10 && d > 0 && d < 10)
            {
                U[h] += u;
                D[h] += d;
                num[h] ++;
                U[t] += u;
                D[t] += d;
                num[t] ++;
            }
        }
    }
    for (unsigned int i = 1; i < N.size(); i ++)
    {
        if (num[i] == 0)
        {
            U[i] = 0;
            D[i] = 0;
        }
        else
        {
            U[i] /= num[i]; 
            D[i] /= num[i];
        }
        fprintf(fout, "%d %.10lf %.10lf\n", i, U[i], D[i]);
    }
    fclose(fout);
    return 0;
}

int    Model::PredGroup(string fileDir)
{
    FILE* fout = fopen(fileDir.c_str(), "w");
    double u = 0, d = 0;
    int cnt = 0;
    for (unsigned int t = 1; t < N.size(); t ++)
    {
        if (U[t] > 0)
            cnt ++;
        u += U[t];
        d += D[t];
    }
    u /= cnt;
    d /= cnt;
    fprintf(fout, "%.10lf %.10lf\n0 %d\n", u, d, N[0]);
    double last_y = N[0];
    for (unsigned int t = 1; t < N.size(); t ++)
    {
        double y = N[t - 1] * pow(u, A[t - 1]) * pow(d, B[t - 1]);
        //double y = N[t - 1] * pow(U[t - 1], A[t - 1]) * pow(D[t - 1], B[t - 1]);
        //double y = last_y * pow(u, A[t - 1]) * pow(d, B[t - 1]);
        //last_y = y;
        //if (t <= 5)
        //    last_y = N[t];
        //if (t % 5 == 0)
        //    last_y = N[t];
        fprintf(fout, "%d %.5lf %d %d\n", t, y, A[t - 1], B[t - 1]);
    }
    fclose(fout);
    return 0;
}

int     Model::PredMicro(vector<Post*> postList)
{
    vector<int> muSum;
    mu.clear();
    for (unsigned int i = 0; i < coreList.size(); i ++)
    {
        mu.push_back(0);
        muSum.push_back(0);
    }
    double maxMu = 0.0;
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        User* user = postList[i] -> user;
        int uid = user -> id;
        if (coreSet.count(uid) == 0)
            continue;
        int cid = coreMap[uid];
        mu[cid] ++;
        muSum[cid] ++;
        for (unsigned int j = 0; j < user -> followerList.size(); j ++)
        {
            int fid = user -> followerList[j] -> id;
            if (coreSet.count(fid) == 0)
                continue;
            int cfid = coreMap[fid];
            muSum[cfid] ++;
        }
    }
    double a = 0.0, b = 0.0;
    int cnt = 0;
    for (unsigned int i = 0; i < mu.size(); i ++)
    {
        mu[i] = mu[i] / (muSum[i] + eps);
        if (mu[i] > 0.9 || mu[i] == 0)
            continue;
        if (mu[i] > maxMu)
            maxMu = mu[i];
        a += mu[i];
        b += mu[i] * (1 - mu[i]);
        cnt ++;
    }
    printf("%.10lf %d\n", maxMu, cnt);
    printf("%.10lf %.10lf\n", a, b);
    return 0;
}
