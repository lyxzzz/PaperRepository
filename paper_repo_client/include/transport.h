#ifndef _TRANSPORT_H
#define _TRANSPORT_H

#define FINI_SYMBOL "^^^fini$$$"
typedef unsigned int uint32;
enum HEADER_STATE{
    ASK_PAPER,
    SEND_PAPER,
    SEND_REPO,
    COMPLETE_PAPER,
    FINI,
    SEND_PASSWD,
};

struct PAPER;
struct REPOSITORY;
int fini(uint32 fd);
int asksendpaper(uint32 fd, const string& name);
string recvstatepkg(uint32 fd);

int sendpaper(uint32 fd, PAPER& paper, string& prefix);
int recvpaper(uint32 fd, PAPER& paper);

int sendrepository(uint32 fd, const REPOSITORY& repo);
int recvrepository(uint32 fd, REPOSITORY& repo);

int sendpasswd(uint32 fd);
int checkpasswd(uint32 fd);
#endif