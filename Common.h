#ifndef CHATROOM_COMMON_H
#define CHATROOM_COMMON_H

#include <iostream>
#include <list>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

using namespace std;

#define SERVER_IP INADDR_ANY
#define SERVER_PORT 8888
#define EPOLL_SIZE 5000
#define BUF_SIZE 65535
#define SERVER_WELCOME "Welcome to the chat room. Your ID is: Client #%d"
#define SERVER_MESSAGE "Client ID %d say >> %s"
#define EXIT "EXIT"
#define CAUTION "You are the only one left in the chatroom!"

static void addfd(int epfd, int fd, bool enable_et){
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(enable_et)
        ev.events |= EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    cout << "fd " << fd << " added to epoll!" << endl << endl;
}

static void err_die(const char *str){
    perror(str);
    exit(EXIT_FAILURE);
}

#endif  //CHATROOM_COMMON_H
