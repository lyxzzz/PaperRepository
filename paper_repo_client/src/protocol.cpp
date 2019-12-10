#include "protocol.h"
#include <winsock2.h>  
#include <stdio.h>

void send_data(uint32 sockfd, void* buf, uint32 len){
    send(sockfd, (char*)buf, len, 0);
}

int recv_data(uint32 sockfd, char* buf, uint32& len){
    int iRet = recv(sockfd, buf, len, 0);
    if(iRet >= 0){
        len = iRet;
    }else{
        len = 0;
    }
    return iRet;
}