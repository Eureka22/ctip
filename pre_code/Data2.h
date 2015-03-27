#pragma once

#include <string>
#include <vector>
#include <set>
#include <map>
#include <cmath>

using namespace std;

class       User
{
public:
    User(int id, string name);
    int             id;
    string          name;
    vector<User*>   followerList;
    vector<User*>   followeeList;
};

class       Post
{
public:
    int             id;
    int             postTime;
    int             contentLength;
    string          name;
    string          content;
    string          source;
    User*           user;
    Post*           repostFrom;
    Post*           sourcePost;
    vector<Post*>   retweetList;
    Post(int id, string name);
};

class   DataLoader
{
public:
    int                 TIME_STEP;
    vector<string>      newInputs;
    vector<User*>       userList;
    vector<Post*>       postList;
    vector<int>         sourceList;
    set<string>         activeUserSet;
    map<int, int>       sourceMap;
    map<string, int>    userIdMap;
    map<string, int>    postIdMap;
    
    DataLoader();
    int                 LoadData(string networkFile, string postFile);
    int                 LoadNetwork(string fileDir);
    int                 LoadContent(string fileDir);
    int                 LoadDiffusion(string fileDir);
    int                 GetSourceId(const int key);
    int                 GetUserId(const string& key);
    int                 GetPostId(const string& key);
    int                 GetOrInsertSourceId(const int key);
    int                 GetOrInsertUserId(const string& key);
    int                 GetOrInsertPostId(const string& key);
};
