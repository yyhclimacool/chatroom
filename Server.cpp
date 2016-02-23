#include <string>
#include <iostream>
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

// 关闭服务，清理并关闭文件描述符
void Server::Close(){
    close(listener);
    close(epfd);
}

// 发送广播消息给所有客户端，发送出错返回-1
int Server::SendBroadcastMessage(int clientfd){
    // Step1:接收新消息，recv()
    char buf[BUF_SIZE], message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);
    cout << "Read from ClientID : " << clientfd << endl;
    int len = recv(clientfd, buf, BUF_SIZE, 0);
    // Step2:判断是否是客户端终止连接
    if(len == 0){
        close(clientfd);
        clients_list.remove(clientfd);
        cout << "ClientID : " << clientfd
            << "closed.\nnow there are " << clients_list.size() 
            << " client(s) in the chat room" << endl;
    }else{
        // Step3:判断是否聊天室还有其他客户端
        if(clients_list.size() == 1){
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return len;
        }
        // Step4:格式化发送的消息内容，sprintf()
        sprintf(message, SERVER_MESSAGE, clientfd, buf);
        // Step5:遍历客户端列表依次发送消息，send()
        for(const auto &elem : clients_list){
            if(elem != clientfd)
                if(send(elem, message, BUF_SIZE,0) < 0)
                    return -1; 
        }
    }
    return len;
}

void Server::Start(){
    // epoll事件队列，里面保存已就绪的事件
    static struct epoll_event events[EPOLL_SIZE];
    Init();
    while(1){
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if(epoll_events_count < 0){
            cerr << "epoll_wait failure" << endl
            break;
        }

        cout << "epoll_events_count = " << epoll_events_count << endl;
        // 处理epoll_events_count个就绪事件
        for(int i = 0; i < epoll_events_count; ++i){
            int sockfd = events[i].data.fd;
            // 处理新用户连接
            if(sockfd == listener){
                struct sockaddr_in client_addr;
                socklen_t client_addrLength = sizeof(sockaddr_in);
                clientfd = accept(listener, (struct sockaddr *)&client_addr, &client_addLength);
                cout << "Client connection from: "
                    << inet_ntoa(client_addr.sin_addr) << " : "
                    << ntohs(client_addr.sin_port) << ", clientfd = " << clientfd << endl;
                addfd(epfd, clientfd, true);
                // 更新服务器端clients_list
                clients_list.push_back(clientfd);
                cout << "Add new clientfd = " << clientfd << " to epoll" << endl;
                cout << "Now there are " << clients_list.size() << " clients in the chat room." << endl;
                cout << "Welcome message : " << endl;
                char message[BUF_SIZE];
                bzero(message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SIZE, 0);
                if(ret < 0){
                    cerr << "send() error." << endl;
                    Close();
                    return -1;
                }
            }
            // 处理用户发来的信息
            else{
                int ret = SendBroadcastMessage(sockfd);
                if(ret < 0){
                    cerr << "SendBroadcastMessage error." << endl;
                    Close();
                    return -1;
                } 
            }
        }
    }
    Close(); 
}




