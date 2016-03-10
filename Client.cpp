#include "Client.h"

using namespace std;

Client::Client(){
    servAddr.sin_family = AF_INET;
    servAddr.sin_port = htons(SERVER_PORT);
    servAddr.sin_addr.s_addr = inet_addr(SERVER_IP);

    sock = 0;
    pid = 0;
    isClientWork = true;
    epfd = 0;
}

void Client::Connect(){
    cout << "Connect server " << SERVER_IP << " : " << SERVER_PORT << endl;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    if(sock < 0){
        perror("create socket error");
        exit(-1);
    }
    if(connect(sock, (struct sockaddr *)&servAddr, sizeof(servAddr)) < 0){
        perror("connect error");
        exit(-1);
    }
    if(pipe(pipe_fd) < 0){
        perror("pipe error");
        exit(-1);
    }
    epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0){
        perror("epfd error");
        exit(-1);
    }
    addfd(epfd, sock, true);
    addfd(epfd, pipe_fd[0], true);
}

void Client::Close(){
    if(pid){
        close(pipe_fd[0]);
        close(sock);
    }else{
        close(pipe_fd[1]);
    }
}

void Client::Start(){
    static struct epoll_event events[2];
    Connect();
    pid = fork();
    if(pid < 0){
        perror("fork error");
        close(sock);
        exit(-1);
    }else if(pid == 0){
        close(pipe_fd[0]);
        cout << "Plese input 'EXIT' to exit the chat room" << endl;
        while(isClientWork){
            bzero(message, BUF_SIZE);
            fgets(message, BUF_SIZE, stdin);
            if(strncasecmp(message, EXIT, strlen(EXIT)) == 0){
                isClientWork = 0;
            }else{
                if(write(pipe_fd[1], message, strlen(message) - 1) < 0){
                    perror("write pipe error");
                    exit(-1);
                }
            } 
    }else{
        close(pipe_fd[1]);
        while(isClientWork){
             
        }
    }
}
