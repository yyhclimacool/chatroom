#include <string>
#include "Server.h"

using namespace std;

Server::Server(){
    // 初始化服务器地址和端口
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    listener = 0;
    epfd = 0;
    clients_list = {};
}

// 初始化服务端并启动监听
void Server::Init(){
    cout << "Init Server ... " << endl;
    // Step1: 创建监听socket，使用socket()
    listener = socket(PF_INET, SOCK_STREAM, 0)
    if(listener < 0) { perror("create socket failed."); return -1; }

    // Step2: 绑定地址，使用bind()
    if(bind(listener, (struct sock_addr *)&serverAddr, sizeof(serverAddr)) < 0){
        perror("bind error.");
        return -1;
    }

    // Step3: 监听连接，使用listen()
    int ret = listen(listener, 5);
    if(ret < 0) {
        perror("listen error.");
        return -1;
    }
    cout << "Start to listen: " << SERVER_IP << " : " << SERVER_PORT << endl;

    // Step4: 创建事件表，使用epoll_create()
    epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0){
        perror("epoll_create failed.");
        return -1;
    }

    // Step5: 添加监听fd到事件表epfd中，使用Common.h中的addfd()
    addfd(epfd, listener, 1);
}



