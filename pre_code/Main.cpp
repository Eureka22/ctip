#include "Util.h"
#include "Data2.h"
#include "Sampler.h"
#include "Model.h"

#include <sstream>
#include <fstream>
#include <cstdio>

using namespace std;

int     main()
{
    int TIME_STEP = 3600; // m: 60, h: 3600
    string NETWORK_FILE_DIR = "/mnt/hd2/Tencent_Follow_data/Tencent_Follow_data/";
    string DIFFUSION_FILE_DIR = "/mnt/hd2/Tencent_Retweet_Data/Tencent_Retweet_Data/";
    //string DIFFUSION_FILE_DIR = "hot_posts5.txt";

    DataLoader* dataLoader = new DataLoader();
    dataLoader -> TIME_STEP = TIME_STEP;
    dataLoader -> LoadData(NETWORK_FILE_DIR, DIFFUSION_FILE_DIR);
    //dataLoader -> LoadDiffusion(DIFFUSION_FILE_DIR);

    Sampler* sampler = new Sampler(dataLoader);
    sampler -> SampleHotPost(30);

    //Model* model = new Model();
    //model -> Load(dataLoader);
    return 0;
}

