#ifndef CHATROOM_CLIENT_H
#define CHATROOM_CLIENT_H

#include "Common.h"

using namespace std;

class Client{
public:
    Client();
    void Connect();
    void Close();
    void Start();
private:
    int sock;
    int pid;
    int epfd;
    int pipe_fd[2];
    bool isClientWork;
    char message[BUF_SIZE];
    struct sockaddr_in servAddr;
};

#endif // CHATROOM_CLIENT_H
