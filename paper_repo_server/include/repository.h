#ifndef _REPOSITORY_H
#define _REPOSITORY_H
#include <map>
#include <set>
#include <string>
#include <iostream>
using std::set;
using std::ostream;
using std::string;
using std::map;
typedef unsigned int uint32;
typedef unsigned long long uint64;

struct PAPERID{
    uint32 head;
    uint32 body;
    string timestamp;
    PAPERID(){}
    PAPERID(string timestamp);
    bool operator<(const PAPERID& others)const;
};

ostream& operator<<(ostream& out, const PAPERID& id);

struct PAPER{
    PAPERID id;
    string path;
    uint64 size;
    char* content;
    PAPER(){}
    bool operator<(const PAPER& others)const;
    void savePaper(const string prefix);
};

struct REPOSITORY{
    set<string> dir_set;
    map<string, PAPER> paper_map;
    string repo_path;
    string config_path;
    REPOSITORY(string repo_path, string config_path);
    REPOSITORY(){};
    void update(uint32 fd, REPOSITORY& dst_repo);
    void save_config();
};
#endif