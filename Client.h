#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

#include "Common.h"

using namespace std;

class Client{
public:
    Client() = delete;
    Client(const char *addrstr);
    void Connect();
    void Close();
    void Start();
private:
    int sockfd;
    int pid;
    int epfd;
    int pipe_fd[2];
    bool isClientWork;
    char message[BUF_SIZE];
    struct sockaddr_in serv_addr;
};

#endif // CHATROOM_CLIENT_H
