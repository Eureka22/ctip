#pragma once
#include "Data.h"

#include <string>
#include <vector>
#include <set>
#include <cmath>

using namespace std;

class   Model
{
public:
    int                 minT, maxT;
    set<int>            coreSet;
    map<int, int>       coreMap;
    vector<int>         coreList;
    vector<double>      mu;
    vector<double>      U, D;   // up&down factor
    vector<int>         N;      // #retweet
    vector<int>         A;      // #active
    vector<int>         B;      // #inactive

    Model();
    int                 SampleCore(vector<Post*> postList);
    int                 Load(vector<Post*> postList, string fileDir);
    int                 Load(DataLoader* dataLoader);
    int                 LearnFactor(string fileDir);
    int                 PredGroup(string fileDir);
    int                 PredMicro(vector<Post*> postList);
};
