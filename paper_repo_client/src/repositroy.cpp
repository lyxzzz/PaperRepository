#include "repository.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include <io.h>
#include <time.h>
#include <fstream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include "transport.h"
#include <direct.h>
#include <windows.h>
using namespace std;

#define SAVE_BLOCK_SIZE 1024 * 8

void set_modify_time(const char* path, PAPERID& id);

ostream& operator<<(ostream& out, const PAPERID& id)
{   
    char timestamp[64] = {0};
    sprintf(timestamp, "%04d/%02d/%02d %02d-%02d-%02d", id.head / 10000, (id.head % 10000) / 100,
        id.head % 100, id.body / 10000, (id.body % 10000) / 100, id.body % 100);
    out << timestamp;
    return out;
}

PAPERID::PAPERID(string timestamp){
    this->head = atoi(timestamp.substr(0, 8).c_str());
    this->body = atoi(timestamp.substr(8).c_str());
    this->timestamp = timestamp;
}

bool PAPERID::operator<(const PAPERID& others)const{
    if(this->head < others.head){
        return true;
    }
    return this->body < others.body;
}

bool PAPER::operator<(const PAPER& others)const{
    return !(this->id < others.id);
}

void PAPER::savePaper(const string prefix){
    string realpath = prefix + "/" + this->path;
    FILE* fp = fopen(realpath.c_str(), "wb");
    uint64 save_size = 0;
    while(save_size < this->size){
        uint32 tmp = SAVE_BLOCK_SIZE < this->size - save_size ? SAVE_BLOCK_SIZE : this->size - save_size;
        tmp = fwrite(&this->content[save_size], sizeof(char), tmp, fp);
        if(tmp > 0){
            save_size += tmp;
        }
    }
    fclose(fp);
    delete this->content;
    this->content = NULL;
    set_modify_time(realpath.c_str(), this->id);
}

string __getTime(time_t t){
    char ch[64] = {0};
    strftime(ch, sizeof(ch) - 1, "%Y%m%d%H%M%S", localtime(&t));
    string result(ch);
	return result;
}

// wchar_t * char_to_wchar(const char* path){
//     size_t len = strlen(path) + 1;
//     size_t converted = 0;
//     wchar_t *wstr;
//     wstr=(wchar_t*)malloc(len*sizeof(wchar_t));
//     mbstowcs(wstr, path, len);
//     return wstr;
// }

void set_modify_time(const char* path, PAPERID& id){
    SYSTEMTIME spec_time;
    sscanf(id.timestamp.c_str(), "%4d%2d%2d%2d%2d%2d", &spec_time.wYear, &spec_time.wMonth, &spec_time.wDay,
            &spec_time.wHour, &spec_time.wMinute , &spec_time.wSecond);
    spec_time.wDayOfWeek        = 1;
    spec_time.wMilliseconds     = 0;
    FILETIME ft, LocalFileTime;
    SystemTimeToFileTime(&spec_time, &ft);
    LocalFileTimeToFileTime(&ft, &LocalFileTime);

    HANDLE h = CreateFile(path, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_DELETE, 
            NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS, NULL);
    SetFileTime(h, (FILETIME*)&LocalFileTime, (FILETIME*)&LocalFileTime, (FILETIME*)&LocalFileTime);
}

void __listFiles(string path,string mode, uint32 prefix, map<string, PAPER>& paper_map, set<string>& dir_set, bool rootdir=false)
{
    _finddata_t file;
    intptr_t HANDLE;
    string Onepath = path + mode;
    HANDLE = _findfirst(Onepath.c_str(), &file);
    if (HANDLE == -1L)
    {
        return;
    }
    do {
        if(file.name[0] == '.'){
            continue;
        }
        string completePath = path +"/" + file.name;

        if (file.attrib & 0x10)
        {
            dir_set.insert(completePath.substr(prefix));
            __listFiles(completePath, mode, prefix, paper_map, dir_set);
        }else{
            PAPER p;
            p.id = PAPERID(__getTime(file.time_write));
            p.path = completePath.substr(prefix);
            paper_map[completePath.substr(prefix)] = p;
        }
    } while (_findnext(HANDLE, &file) == 0);
    _findclose(HANDLE);
}
REPOSITORY::REPOSITORY(string repo_path, string config_path):repo_path(repo_path), config_path(config_path){
    __listFiles(repo_path, "/*.*", repo_path.size() + 1, this->paper_map, this->dir_set, true);
}

void REPOSITORY::save_config(){
    fstream out;
    out.open(this->config_path, ios::out);
    vector<PAPER> vec;
    for(auto it = this->paper_map.begin(); it != this->paper_map.end(); ++it){
        vec.push_back(it->second);
    }
    sort(vec.begin(), vec.end());

    for(auto p:vec){
        out << p.id << ":" << p.path << endl;
    }
    out.close();
}

void REPOSITORY::update(uint32 fd, REPOSITORY& dst_repo){
    for(auto it = dst_repo.dir_set.begin(); it != dst_repo.dir_set.end(); ++it){
        if(this->dir_set.find(*it) == this->dir_set.end()){
            string completePath = this->repo_path + "/" + (*it);
            mkdir(completePath.c_str());
        }
    }

    for(auto it = dst_repo.paper_map.begin(); it != dst_repo.paper_map.end(); ++it){
        if(this->paper_map.find(it->first) == this->paper_map.end() ||
            this->paper_map[it->first].id < it->second.id){
            asksendpaper(fd, it->first);
            this->paper_map[it->first] = PAPER();
            recvpaper(fd, this->paper_map[it->first]);
            this->paper_map[it->first].savePaper(this->repo_path);
        }
    }
    
    fini(fd);
}