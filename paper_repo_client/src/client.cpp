#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <winsock2.h>
#include "repository.h"
#include "transport.h"

#pragma  comment(lib,"ws2_32.lib")

int create_connect(char** argv);
int close_connnect(int socketfd);

int main(int argc, char** argv)
{
    int socketfd = create_connect(argv);
    REPOSITORY repo(argv[3], argv[4]);
    sendpasswd(socketfd);

    REPOSITORY dst_repo;
    recvrepository(socketfd, dst_repo);
    repo.update(socketfd, dst_repo);
    repo.save_config();

    sendrepository(socketfd, repo);
    string receive;
    while(true){
        receive = recvstatepkg(socketfd);
        if(receive == "^^^fini$$$"){
            break;
        }else{
            sendpaper(socketfd, repo.paper_map[receive], repo.repo_path);
        }
    }

    close_connnect(socketfd);
    return 0;
}

int create_connect(char** argv){
    WORD sockVersion = MAKEWORD(2,2);  
    WSADATA data;   
    if(WSAStartup(sockVersion, &data) != 0)  
    {  
        return -1;  
    }  
  
    SOCKET sclient = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);  
    if(sclient == INVALID_SOCKET)  
    {
        return -1;  
    }  
  
    sockaddr_in serAddr;  
    serAddr.sin_family = AF_INET; 
    serAddr.sin_port = htons(atoi(argv[1]));  
    serAddr.sin_addr.S_un.S_addr = inet_addr(argv[2]);   
    if (connect(sclient, (sockaddr *)&serAddr, sizeof(serAddr)) == SOCKET_ERROR)  
    {
        closesocket(sclient);  
        return -1;  
    }
    return sclient;
}

int close_connnect(int socketfd){
    closesocket(socketfd);  
    WSACleanup();
    return 0;
}