#include "Server.h"

using namespace std;

Server::Server(){
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    listener = 0;
    epfd = 0;
}

void Server::Init(){
    cout << "Init server ... " << endl;
    if((listener = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("create socket error");
        exit(-1);
    }
    if(bind(listener, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
        perror("bind error");
        exit(-1);
    }
    int ret = listen(listener, 5);
    if(ret < 0){
        perror("listen error");
        exit(-1);
    }
    cout << "Start to listen : " << SERVER_IP << endl;
    epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0){
        perror("epfd error");
        exit(-1);
    }
    addfd(epfd, listener, true);
}

void Server::Close(){
    close(listener);
    close(epfd);
}

int Server::SendBroadcastMessage(int clientfd){
    char buf[BUF_SIZE], message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    cout << "read from client(client ID = " << clientfd << ")" << endl;
    int len = recv(clientfd, buf, BUF_SIZE, 0);
    if(len == 0){
        close(clientfd);
        clients_list.remove(clientfd);
        cout << "Client ID " << clientfd
            << " closed.\n Now there are "
            << clients_list.size() 
            << " client in the chat room" << endl;
    }else{
        if(clients_list.size() == 1){
            send(clientfd, CAUTION, strlen(CAUTION), 0);
            return len;
        }
        sprintf(message, SERVER_MESSAGE, clientfd, buf);
        for(const auto elem : clients_list){
            if(elem != clientfd)
                if(send(elem, message, BUF_SIZE, 0) < 0)
                    return -1;
        }
    }
    return len;
}

void Server::Start(){
    static struct epoll_event events[EPOLL_SIZE];
    Init();
    while(1){
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if(epoll_events_count < 0){
            perror("epoll wait failure");
            break;
        }
        cout << "epoll_events_count = " << epoll_events_count << endl;
        for(int i = 0; i < epoll_events_count; ++i){
            int sockfd = events[i].data.fd;
            if(sockfd == listener){
                struct sockaddr_in clientAddr;
                socklen_t clientAddrLength = sizeof(struct sockaddr_in);
                int clientfd = accept(listener, (struct sockaddr*)&clientAddr, &clientAddrLength);
                cout << "client connection from: "
                    << inet_ntoa(clientAddr.sin_addr) << " : "
                    << ntohs(clientAddr.sin_port) << ", clientfd = "
                    << clientfd << endl;
                addfd(epfd, clientfd, true);
                clients_list.push_back(clientfd);
                cout << "Add new clientfd = " << clientfd << " to epoll" << endl;
                cout << "Now there are " << clients_list.size() 
                    << " clients in the chat room" << endl;
                cout << "Welcome message" << endl;
                char message[BUF_SIZE];
                bzero(message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SIZE, 0);
                if(ret < 0){
                    perror("send error");
                    Close();
                    exit(-1);
                }
            }else{
                int ret = SendBroadcastMessage(sockfd);
                if(ret < 0){
                    perror("send broadcast message error");
                    Close();
                    exit(-1);
                } 
            }
        }

    } 
    Close();
}




