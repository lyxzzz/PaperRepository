#include "repository.h"
#include "transport.h"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <string.h>
#include "protocol.h"
using namespace std;
typedef unsigned long long uint64;
#define MAX_PKG_SIZE 4096
#define MAX_READ_SIZE 1024 * 16

#define PASSWDPREFIX "zcvkjnadfjewoifjoiwqefkdsalnlfhasdf"
#define PASSWD "yourpasswd"

static char send_buf[MAX_PKG_SIZE];
static char read_buf[MAX_READ_SIZE];

struct SEG_HEADER{
    uint32 state;
    uint64 len;
};

void __send(uint32 fd, const char* buf, uint64 len, SEG_HEADER h){
    h.len = len;
    memset(send_buf, 0, sizeof(send_buf));

    memcpy(send_buf, &h, sizeof(h));

    uint64 now_size = sizeof(h);
    uint64 rest_size = MAX_PKG_SIZE - now_size;
    uint64 data_index = 0;
    while(len > 0){
        uint64 send_size = len < rest_size ? len : rest_size;

        memcpy(&send_buf[now_size], &buf[data_index], send_size);
        len -= send_size;
        data_index += send_size;

        send_data(fd, send_buf, send_size + now_size);
        now_size = 0;
        rest_size = MAX_PKG_SIZE;
    }
}

int __recv(uint32 fd, char* buf, uint64& len){
    uint64 recv_size = 0;
    int iRet;
    while(recv_size < len){
        uint32 tmp = len - recv_size  > MAX_PKG_SIZE ? MAX_PKG_SIZE : len - recv_size;
        iRet = recv_data(fd, &buf[recv_size], tmp);
        recv_size += tmp;
    }
    return recv_size;
}

void __send_str(uint32 fd, const string& str, SEG_HEADER h){
    __send(fd, str.c_str(), str.size(), h);
}

void __send_size(uint32 fd, uint64 size, SEG_HEADER h){
    __send(fd, (const char*)&size, sizeof(size), h);
}

uint64 __recv_size(uint32 fd){
    uint64 recv_len = sizeof(SEG_HEADER);
    __recv(fd, send_buf, recv_len);
    uint64 result;
    recv_len = sizeof(result);
    __recv(fd, (char*)&result, recv_len);
    return result;
}

string __recv_str(uint32 fd){
    uint64 recv_len = sizeof(SEG_HEADER);
    __recv(fd, send_buf, recv_len);
    uint64 str_len = ((SEG_HEADER*)send_buf)->len;
    char* str_buf = new char[str_len + 1];
    str_buf[str_len] = 0;
    __recv(fd, str_buf, str_len);
    string result(str_buf);
    delete str_buf;
    return result;
}

void __recv_buf(uint32 fd, char* buf, uint64& len){
    uint64 recv_len = sizeof(SEG_HEADER);
    __recv(fd, send_buf, recv_len);
    uint64 str_len = ((SEG_HEADER*)send_buf)->len;
    __recv(fd, buf, str_len);
    len -= str_len;
}

int sendpaper(uint32 fd, PAPER& paper, string& prefix){
    SEG_HEADER h;
    h.state = SEND_PAPER;
    __send_str(fd, paper.id.timestamp, h);
    __send_str(fd, paper.path, h);

    string realpath = prefix + "\\" + paper.path;
    FILE* fp = fopen(realpath.c_str(), "rb");
    fseek(fp, 0, SEEK_END); //定位到文件末
    uint64 nFileLen = ftell(fp);
    __send_size(fd, nFileLen, h);
    fseek(fp, 0, SEEK_SET);

    printf("send paper:%s, size is %d\n", paper.path.c_str(), nFileLen);

    uint64 read_size = 0;
    while((read_size = fread(read_buf, sizeof(char), MAX_READ_SIZE, fp)) > 0){
        read_buf[read_size] = 0;
        __send(fd, read_buf, read_size, h);
    }
    fclose(fp);
    return 0;
}

int recvpaper(uint32 fd, PAPER& paper){
    string timestamp = __recv_str(fd);
    paper.id = PAPERID(timestamp);
    paper.path = __recv_str(fd);

    paper.size = __recv_size(fd);

    paper.content = new char[paper.size];

    uint64 rest_size = paper.size;
    printf("recv paper:%s, size is %d\n", paper.path.c_str(), paper.size);
    while(rest_size > 0){
        __recv_buf(fd, &paper.content[paper.size-rest_size], rest_size);
    }
    return 0;
}

int asksendpaper(uint32 fd, const string& name){
    SEG_HEADER h;
    h.state = ASK_PAPER;
    __send_str(fd, name, h);
    return 0;
}

int fini(uint32 fd){
    SEG_HEADER h;
    h.state = FINI;
    __send_str(fd, string(FINI_SYMBOL), h);
    return 0;
}

string recvstatepkg(uint32 fd){
    string str = __recv_str(fd);
    return str;
}

int sendrepository(uint32 fd, const REPOSITORY& repo){
    SEG_HEADER h;
    h.state = SEND_REPO;
    __send_size(fd, repo.dir_set.size(), h);
    for(auto str:repo.dir_set){
        __send_str(fd, str, h);
    }

    __send_size(fd, repo.paper_map.size(), h);

    for(auto it = repo.paper_map.begin(); it != repo.paper_map.end(); ++it){
        __send_str(fd, it->first, h);
        __send_str(fd, it->second.id.timestamp, h);
    }

    return 0;
}

int recvrepository(uint32 fd, REPOSITORY& repo){
    uint64 size = __recv_size(fd);
    for(uint64 i = 0; i < size; ++i){
        string str = __recv_str(fd);
        repo.dir_set.insert(str);
    }

    size = __recv_size(fd);
    for(uint64 i = 0; i < size; ++i){
        string str = __recv_str(fd);
        string stamp = __recv_str(fd);
        repo.paper_map[str] = PAPER();
        repo.paper_map[str].id = PAPERID(stamp);
    }

    return 0;
}

int sendpasswd(uint32 fd){
    SEG_HEADER h;
    h.state = SEND_PASSWD;

    string pwd = string(PASSWDPREFIX) + string(PASSWD);
    __send_str(fd, pwd, h);
    return 0;
}

int checkpasswd(uint32 fd){
    string pwd = string(PASSWDPREFIX) + string(PASSWD);
    uint64 recv_len = sizeof(SEG_HEADER);
    __recv(fd, send_buf, recv_len);
    if(((SEG_HEADER*)send_buf)->state != SEND_PASSWD || ((SEG_HEADER*)send_buf)->len != pwd.size()){
        return -1;
    }
    uint64 str_len = ((SEG_HEADER*)send_buf)->len;
    char* str_buf = new char[str_len + 1];
    str_buf[str_len] = 0;
    __recv(fd, str_buf, str_len);
    string result(str_buf);
    delete str_buf;
    if(result == pwd){
        return 0;
    }else{
        return -1;
    }
}