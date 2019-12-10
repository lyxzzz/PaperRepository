#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include <set>
#include <sys/epoll.h>
#include "repository.h"
#include "transport.h"
using std::set;

#define LISTEN_NUM   1
#define BUFFER_SIZE 1024

int create_listen(char** argv, int& epollfd);
int process(int epollfd, int listenfd, REPOSITORY& repo);
int close_server(int sockfd, int epollfd);

set<uint32> ban_list;

int main(int argc, char** argv)
{
    REPOSITORY repo("repository", "config.repo");
    int epollfd;
    int socketfd = create_listen(argv, epollfd);
    while(true){
        process(epollfd, socketfd, repo);
    }
    close_server(socketfd, epollfd);
    return 0;
}

int start_a_new_repo(int fd, REPOSITORY& repo){
    sendrepository(fd, repo);
    string receive;
    while(true){
        receive = recvstatepkg(fd);
        if(receive == "^^^fini$$$"){
            break;
        }else{
            sendpaper(fd, repo.paper_map[receive], repo.repo_path);
        }
    }

    REPOSITORY dst_repo;
    recvrepository(fd, dst_repo);

    repo.update(fd, dst_repo);
    repo.save_config();
}

int process(int epollfd, int listenfd, REPOSITORY& repo){
    struct epoll_event events_in[20];

    int nfd = epoll_wait(epollfd, events_in, sizeof(events_in), -1);
    for (int i = 0; i < nfd; i++)
    {
        if (events_in[i].data.fd == listenfd)  //新连接请求
        {
            struct sockaddr_in client_addr;
            socklen_t length = sizeof(client_addr);
            int new_fd = accept(listenfd, (struct sockaddr*)&client_addr, &length);

            if(ban_list.find(client_addr.sin_addr.s_addr) != ban_list.end()){
                close(new_fd);
            }else{
                if(checkpasswd(new_fd) >= 0){
                    start_a_new_repo(new_fd, repo);
                }else{
                    ban_list.insert(client_addr.sin_addr.s_addr);
                }
                close(new_fd);
            }
        }
    }
    return 0;
}

int create_listen(char** argv, int& epollfd){
    int MYPORT = atoi(argv[1]);
    int server_sockfd = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_sockaddr;
    server_sockaddr.sin_family = AF_INET;
    server_sockaddr.sin_port = htons(MYPORT);
    server_sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if(bind(server_sockfd,(struct sockaddr *)&server_sockaddr,sizeof(server_sockaddr))==-1)
    {
        return -1;
    }

    if(listen(server_sockfd, LISTEN_NUM) == -1)
    {
        return -1;
    }

    epollfd = epoll_create(16);
    if(epollfd < 0){
        return -1;
    }

    struct epoll_event event;
    event.events = EPOLLIN;
    event.data.fd = server_sockfd;

    int iret = epoll_ctl(epollfd, EPOLL_CTL_ADD, server_sockfd, &event);
    if(iret < 0){
        return -1;
    }
    return server_sockfd;
}

int close_server(int sockfd, int epollfd){
    close(sockfd);
    close(epollfd);
}