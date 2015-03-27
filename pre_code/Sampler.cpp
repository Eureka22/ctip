#include "Util.h"
#include "Data2.h"
#include "Sampler.h"
#include <sstream>
#include <cstdio>
#include <vector>
#include <algorithm>

using namespace std;

bool    cmp(pair<int, int> a, pair<int, int> b)
{
    return a.second > b.second;
}

int     Sampler::PrintPosts(vector<int> postList, string fileDir)
{
    FILE* fout = fopen(fileDir.c_str(), "w");
    vector<string> tokens;
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[postList[i]];
        fprintf(fout, "%s", post -> source.c_str());
    }
    fclose(fout);
    return 0;
}

// sample top K source posts with most retweet num
int     Sampler::SampleHotPost(int topK)
{
    printf("Start sampling!\n");
    vector<pair<int, int> > countList; //(post_id, retweet_num)
    int error = 0;
    for (unsigned int i = 0; i < dataLoader -> postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[i];
        int sid = dataLoader -> GetSourceId(post -> sourcePost -> id);
        //printf("%d",sid);
        if (sid == -1)
        {
            error ++;
            continue;
        }
        for (unsigned int j = countList.size(); (int) j <= sid; j ++)
        {
            countList.push_back(make_pair(dataLoader -> sourceList[j], 0));
        }
        if (post->sourcePost != post->repostFrom)
        {
           // printf("sp%d rp%d \n",post->sourcePost,post->repostFrom);
            countList[sid].second ++;
        }
    }
    if (error > 0)
        printf("Can not find %d source posts!\n", error);
    sort(countList.begin(), countList.end(), cmp);
    set<int> sampledSourceSet;
    for (int i = 0; i < topK; i ++)
    {
        printf("%d %d\n", countList[i].first, countList[i].second);
        sampledSourceSet.insert(countList[i].first);
    }
    vector<int> sampledPostList;
    for (unsigned int i = 0; i < dataLoader -> postList.size(); i ++)
    {
        Post* post = dataLoader -> postList[i];
        int sid = post -> sourcePost -> id;
        if (sampledSourceSet.count(sid) != 0)
            sampledPostList.push_back(i);
    }
    printf("Sampled %d posts in total.\n", (int) sampledPostList.size());
    PrintPosts(sampledPostList, "hot_posts" + Util::Int2Str(topK) + ".txt");
    return 0;
}

Sampler::Sampler(DataLoader* dataLoader)
{
    this -> dataLoader = dataLoader;
}

