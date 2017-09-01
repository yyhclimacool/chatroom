#include "Client.h"

using namespace std;

Client::Client(const char *addrstr):sockfd(0), pid(0), epfd(0){
    isClientWork = true;
    // initialize the structure
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    int ret = inet_aton(addrstr, &serv_addr.sin_addr);
    if(ret == 0)
        err_die("inet_aton failed.");
}

void Client::Connect(){
    string server_ip(inet_ntoa(serv_addr.sin_addr));
    cout << "Connect server " << server_ip << " : " << SERVER_PORT << endl;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
        err_die("socket error");
    if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        err_die("connect error");
    if(pipe(pipe_fd) < 0)
        err_die("pipe error");
    epfd = epoll_create(EPOLL_SIZE);
    if(epfd < 0)
        err_die("epoll_create error");
    addfd(epfd, sockfd, true);
    addfd(epfd, pipe_fd[0], true);
}

void Client::Close(){
    if(pid){
        close(pipe_fd[0]);
        close(sockfd);
    }
    else    // in child process
        close(pipe_fd[1]);
}

void Client::Start(){
    static struct epoll_event events[2];
    Connect();
    pid = fork();
    if(pid < 0){    // fork child process failed
        close(sockfd);
        err_die("fork error");
    }
    else if(pid == 0){    // in child process
        close(pipe_fd[0]);    // Close the read pipe
        cout << "Please input 'EXIT' to exit the chat room." << endl;
        while(isClientWork){
            bzero(&message, BUF_SIZE);
            fgets(message, BUF_SIZE, stdin);
            if(strncasecmp(message, EXIT, strlen(EXIT)) == 0)
                isClientWork = 0;
            else{
                if(writen(pipe_fd[1], message, strlen(message) - 1) < 0)    // write to pipe
                    err_die("write error");
            }
        }
    }
    else{
        close(pipe_fd[1]);
        while(isClientWork){
            int epoll_event_count = epoll_wait(epfd, events, 2, -1);
            for(int i = 0; i < epoll_event_count; ++i){
                bzero(&message, BUF_SIZE);
                if(events[i].data.fd == sockfd){
                    int ret = recv(sockfd, message, BUF_SIZE, 0);
                    if(ret == 0){
                        cout << "Server closed connection: " << endl;
                        close(sockfd);
                        isClientWork = 0;
                    }
                    else
                        cout << message << endl;
                }
                else{
                    int ret = read(events[i].data.fd, message, BUF_SIZE);
                    if(ret == 0)
                        isClientWork = 0;
                    else
                        send(sockfd, message, BUF_SIZE, 0);
                }
            }
        }
    }
    Close();
}
