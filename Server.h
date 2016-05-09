#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H
#include "Common.h"

using namespace std;

class Server{
public:
    Server();
    void Init();
    void Start();
    void Close();
private:
    int serv_sock;
    int epfd;
    list<int> clients_list;
    struct sockaddr_in serv_addr;

    int SendBroadcastMessage(int clientfd);
};

#endif // CHATROOM_COMMON_H
