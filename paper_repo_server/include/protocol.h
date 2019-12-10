#ifndef _PROTOCOL_H
#define _PROTOCOL_H

typedef unsigned int uint32;
void send_data(uint32 sockfd, void* buf, uint32 len);
int recv_data(uint32 sockfd, char* buf, uint32& len);
#endif