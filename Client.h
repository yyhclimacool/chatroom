#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

#include "Common.h"

using namespace std;

class Client{
public:
    // No default construction
    Client() = delete;
    // addrstr identical to 192.168.1.100 in string
    Client(const char *addrstr);
    void Connect();
    void Close();
    void Start();
private:
    int sockfd;
    // every client have a pid
    int pid;
    // every client have a epoll handler
    int epfd;
    // unnamed pipe
    int pipe_fd[2];
    bool isClientWork;
    char message[BUF_SIZE];
    // server address structure, initialized in constructor
    struct sockaddr_in serv_addr;
};

#endif // CHATROOM_CLIENT_H
