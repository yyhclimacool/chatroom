#ifndef CHATROOM_SERVER_H
#define CHATROOM_SERVER_H

#include "Common.h"
#include <string>

using namespace std;

class Server{
public:
    Server();
    // 初始化服务端并启动监听
    void Init();
    // 关闭所有打开的文件描述符
    void Close();
    // 启动服务端
    void Start();
private:
    // 创建监听的socket
    int listener;
    // epoll_create创建后的返回值，即epoll句柄，Linux内核中创建的事件表
    int epfd;
    // 向所有客户端发送广播信息
    int SendBroadcastMessage(int clientfd);
    // 保存所有已连接的客户端
    list<int> clients_list;
    // 服务器端地址
    struct sockaddr_in serverAddr;
};


#endif // CHATROOM_SERVER_H
