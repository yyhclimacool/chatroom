#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H

#include "Common.h"
#include <string>

using namespace std;

class Server{
public:
    Server();
    void Init();
    void Close();
    void Start();
private:
    struct sockaddr_in servAddr;
    int listener;
    int epfd;
    list<int> clients_list{};
    int SendBroadcastMessage(int clientfd);
};

#endif //CHATROOM_SERVER_H
