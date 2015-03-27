#pragma once

#include <string>
#include <vector>
#include "Data2.h"

using namespace std;

class   Sampler
{
public:
    DataLoader* dataLoader;
    Sampler(DataLoader* dataLoader);
    int     SampleHotPost(int topK);
    int     PrintPosts(vector<int> postList, string fileDir);
};
