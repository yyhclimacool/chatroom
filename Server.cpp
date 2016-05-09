#include "Server.h"

using namespace std;

Server::Server():serv_sock(0), epfd(0), clients_list(list<int>{}){
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    // inet_addr was depricated
    // serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    serv_addr.sin_addr.s_addr = htonl(SERVER_IP);
}

void Server::Init(){
    cout << "Init server... " << endl;
    if((serv_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
        err_die("socket error");
    if(bind(serv_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        err_die("bind error");
    int ret = listen(serv_sock, 5);
    if(ret < 0)
        err_die("listen error");
    cout << "Start to listen on port " << SERVER_PORT << endl;
    epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0)
        err_die("epoll_create error");
    addfd(epfd, serv_sock, true);
}

void Server::Close(){
    close(serv_sock);
    close(epfd);
}

int Server::SendBroadcastMessage(int clientfd){
    char buf[BUF_SIZE], message[BUF_SIZE];
    bzero(buf, BUF_SIZE);
    bzero(message, BUF_SIZE);

    cout << "read from client(ID = " << clientfd << ")" << endl;
    int len = recv(clientfd, buf, BUF_SIZE, 0);
    if(len == 0){
        close(clientfd);
        clients_list.remove(clientfd);
        cout << "Client ID " << clientfd
            << " closed .\n Now there are "
            << clients_list.size()
            << " clients in the chat room" << endl;
    }
    else{
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
    Init();
    static struct epoll_event events[EPOLL_SIZE];
    while(1){
        int epoll_events_count = epoll_wait(epfd, events, EPOLL_SIZE, -1);
        if(epoll_events_count < 0){
            perror("epoll wait failure");
            break;
        }
        cout << "epoll events count = " << epoll_events_count << endl;
        for(int i = 0; i < epoll_events_count; i++){
            int sockfd = events[i].data.fd;
            if(sockfd == serv_sock){
                struct sockaddr_in client_addr;
                socklen_t client_len = sizeof(client_addr);
                int clientfd = accept(serv_sock, (struct sockaddr*)&client_addr, &client_len); 
                cout << "Client connection from: "
                    << inet_ntoa(client_addr.sin_addr) << " : "
                    << ntohs(client_addr.sin_port) << ", clientfd = "
                    << clientfd << endl;
                addfd(epfd, clientfd, true);
                clients_list.push_back(clientfd);
                cout << "Now there are " << clients_list.size() 
                    << " clients in the chat server." << endl;
                char message[BUF_SIZE];
                bzero(&message, BUF_SIZE);
                sprintf(message, SERVER_WELCOME, clientfd);
                int ret = send(clientfd, message, BUF_SIZE, 0);
                if(ret < 0){
                    perror("send() error");
                    Close();
                    exit(-1);
                }
            }
            else{
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



















