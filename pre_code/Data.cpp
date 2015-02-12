#include "Util.h"
#include "Data.h"

#include <algorithm>
#include <sstream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <utility>
#include <fstream>
#include <iostream>
#include <dirent.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <cmath>

#define MAXINT 10000000
#define MIN_POST_APPEARENCE 5

using namespace std;

User::User(int id, string name)
{
    this -> id = id;
    this -> name = name;
    followerList.clear();
    followeeList.clear();
}

Post::Post(int id, string name)
{
    this -> id = id;
    this -> name = name;
    this -> content = "";
    retweetList.clear();
}

DataLoader::DataLoader()
{
    TIME_STEP = 1;
    userIdMap.clear();
    postIdMap.clear();
    sourceList.clear();
    sourceMap.clear();
    userList.clear();
    postList.clear();
}

int             DataLoader::GetUserId(const string& key)
{
    map<string, int>::iterator it = userIdMap.find(key);
    if (it != userIdMap.end())
        return it->second;
    return -1;
}

int				DataLoader::GetOrInsertUserId(const string& key)
{
    map<string, int>::iterator it = userIdMap.find(key);
    if (it != userIdMap.end())
        return it -> second;
    int id = (int) userList.size();
    User* user = new User(id, key);
    userList.push_back(user);
    userIdMap.insert(make_pair(key, id));
	return id;
}

int             DataLoader::GetPostId(const string& key)
{
    map<string, int>::iterator it = postIdMap.find(key);
    if (it != postIdMap.end())
        return it -> second;
    return -1;
}

int             DataLoader::GetSourceId(const int key)
{
    map<int, int>::iterator it = sourceMap.find(key);
    if (it != sourceMap.end())
        return it -> second;
    return -1;
}

int             DataLoader::GetOrInsertSourceId(const int key)
{
    map<int, int>::iterator it = sourceMap.find(key);
    if (it != sourceMap.end())
        return it -> second;
    int id = sourceList.size();
    sourceList.push_back(key);
    sourceMap.insert(make_pair(key, id));
	return id;
}

int             DataLoader::GetOrInsertPostId(const string& key)
{
    map<string, int>::iterator it = postIdMap.find(key);
    if (it != postIdMap.end())
        return it -> second;
    int id = (int) postList.size();
    Post* post = new Post(id, key);
    postList.push_back(post);
    post -> sourcePost = NULL;
    post -> postTime = 0;
    postIdMap.insert(make_pair(key, id));
	return post -> id;
}

int             DataLoader::LoadNetwork(string fileDir)
{
    vector<string> inputs = Util::ReadFromFile(fileDir.c_str());
    int edgeCnt = 0;
    int proc = -1;
    for (unsigned int i = 0; i < inputs.size(); i ++)
    {
        if ((int) (i * 100 / inputs.size()) > proc)
        {
            proc = i * 100 / inputs.size();
            //printf("Processing %d%%...\n", proc);
        }
        string new_line = "";
        bool found = false;
        vector<string> tokens = Util::StringTokenize(inputs[i]);
        int source = GetUserId(tokens[0]);
        //int source = GetOrInsertUserId(tokens[0]);
        new_line += tokens[0];
        for (unsigned int j = 1; j < tokens.size(); j ++)
        {
            //int target = GetOrInsertUserId(tokens[j]);
            //if (activeUserSet.count(tokens[j]) == 0)
            //    continue;

            // temporally!!!!!
            int target = GetUserId(tokens[j]);
            if (target == -1)
                continue;
            found = true;
            if (source == -1)
                source = GetOrInsertUserId(tokens[0]);
            new_line += " " + tokens[j];
            userList[source] -> followeeList.push_back(userList[target]);
            userList[target] -> followerList.push_back(userList[source]);
            edgeCnt ++;
        }
        if (found)
            newInputs.push_back(new_line);
    }
    inputs.clear();
    return 0;
}

bool            pairFirstCmp(pair<int, int> a, pair<int, int> b)
{
    return a.first < b.first;
}

int             DataLoader::LoadDiffusion(string fileDir)
{
    vector<string> inputs = Util::ReadFromFile(fileDir.c_str());
    for (unsigned int i = 0; i < inputs.size(); i ++)
    {
        vector<string> tokens = Util::StringTokenize(inputs[i]);
        //if (tokens.size() < 8)
        //    continue;
        /*
        if (! (tokens[3].length() > 1 && tokens[4].length() > 1 && tokens[5].length() > 1 && tokens[6].length() > 1))
            continue;
        */
        // load posting time
        int t = Util::String2Int(tokens[1]);
        // load user ID
        int uid = GetOrInsertUserId(tokens[2]);
        /*
        if (uid == -1)
            continue;
        */
        int type = Util::String2Int(tokens[8]);
        /*
        if (type != 0 && type != 1 && type != 2 && type != 4 && type != 7)
        {
            //printf("Error!\n");
            continue;
        }
        */
        if (tokens[3].length() > 1 && GetPostId(tokens[3]) == -1)
            continue;
        if (tokens[5].length() > 1 && GetPostId(tokens[5]) == -1)
            continue;
        int pid = GetOrInsertPostId(tokens[0]);
        int len = (int) Util::StringTokenize(tokens[7]).size();
        //if (activeUserSet.count(tokens[2]) == 0)
        //    activeUserSet.insert(tokens[2]);
        postList[pid] -> user = userList[uid];
        postList[pid] -> postTime = t / TIME_STEP;
        postList[pid] -> contentLength = len;
        postList[pid] -> name = tokens[0];
        int sid = pid;
        if (tokens[3].length() > 1)
        {
            sid = GetPostId(tokens[3]);
            if (sid == -1)
                continue;
        }
        GetOrInsertSourceId(sid);
        postList[pid] -> sourcePost = postList[sid];

        int fid = pid;
        if (tokens[5].length() > 1)
        {
            fid = GetPostId(tokens[5]);
            if (fid == -1)
                continue;
        }
        postList[pid] -> repostFrom = postList[fid];
        if (fid != pid)
        {
            postList[fid] -> retweetList.push_back(postList[pid]);
            // for analysis only!!!
            //if (fid != sid)
            //postList[sid] -> retweetList.push_back(postList[pid]);
        }
        postList[pid] -> source = inputs[i];
        /*
        for (unsigned int i = 0; i < tokens.size(); i ++)
        {
            if (i > 0)
                postList[sid] -> source += "\t";
            postList[sid] -> source += tokens[i];
        }
        */
    }
    return 0;
}

int         DataLoader::LoadContent(string fileDir)
{
    vector<string> inputs = Util::ReadFromFile(fileDir.c_str());
    for (unsigned int i = 0; i < inputs.size(); i ++)
    {
        vector<string> tokens = Util::StringTokenize(inputs[i]);
        int pid = Util::String2Int(tokens[0]);
        int sid = GetSourceId(pid);
        if (sid == -1)
            continue;
        postList[sid] -> content = tokens[7];

    }
    return 0;
}

int         DataLoader::LoadData(string networkFile, string postFile)
{
    /*
    LoadDiffusion(postFile);
    LoadNetwork("network_10hot_part_0.in");
    */
    //return 0;
    //LoadDiffusion("/home/yang/projects/role-aware/data/tencent/diffusion/10percentage_retweet.txt");
    //LoadDiffusion("retweet.txt");
    //LoadNetwork("/home/yang/projects/role-aware/data/tencent/network/10percentage_network.txt");
    //return 0;
    int D = 1;
    int F = 1;
    //int N = 2028;
    int N = 10;
    newInputs.clear();
    activeUserSet.clear();
    for (int day = 1; day <= D; day ++)
    {
	    string tmp = Util::Int2Str(day);
	    if (day < 10)
		    tmp = "0" + tmp;
    	for (int i = 0; i < F; i ++)
    	{
        	string tmp2 = Util::Int2Str(i);
        	if (i < 10)
            	tmp2 = "0" + tmp2;
        	LoadDiffusion(postFile + "201110" + tmp + "/Test_" + tmp2 + ".txt");
            //LoadContent(postFile + "201110" + tmp + "/Test_" + tmp2 + ".txt");
    	}
    }


    /*
    FILE* fout2 = fopen("retweet.txt", "w");
    vector<string> tokens;
    for (unsigned int i = 0; i < postList.size(); i ++)
    {
        postList[i] -> contentLength = postList[i] -> content.length();
        tokens = Util::StringTokenize(postList[i] -> source);
        for (unsigned int j = 0; j < tokens.size(); j ++)
        {
            if (j == 7)
                fprintf(fout2, "\t%s", postList[i] -> content.c_str());
            if (j > 0)
                fprintf(fout2, "\t");
            fprintf(fout2, "%s", tokens[j].c_str());
        }
        fprintf(fout2, "\n");
    }
    fclose(fout2);
    */

    for (int i = 0; i <= F; i ++)
    {
        string tmp = Util::Int2Str(i);
        if (i < 1000)
            tmp = "0" + tmp;
        if (i < 100)
            tmp = "0" + tmp;
        if (i < 10)
            tmp = "0" + tmp;
        LoadNetwork(networkFile + "part-0" + tmp);
       //LoadNetwork("../marki/network_" + Util::Int2Str(i+1) + ".in");
    }

    FILE* fout = fopen("network_2hot_part_0.in", "w");
    for (unsigned int i = 0; i < newInputs.size(); i ++)
        fprintf(fout, "%s\n", newInputs[i].c_str());
    fclose(fout);

    printf("Load %d user in total.\n", (int) userList.size());
    printf("Load %d posts in total.\n", (int) postList.size());
    printf("Load %d source posts in total.\n", (int) sourceList.size());
    printf("Dataset is ready!\n");
    return 0;
}


