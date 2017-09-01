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

#define MAXLINE 4096

#define SERVER_IP INADDR_ANY
#define SERVER_PORT 8888
#define EPOLL_SIZE 5000
#define BUF_SIZE 65535
#define SERVER_WELCOME "Welcome to the chat room. Your ID is: Client #%d"
#define SERVER_MESSAGE "Client ID %d say >> %s"
#define EXIT "EXIT"
#define CAUTION "You are the only one left in the chatroom!"

// when calling this function, we assume you already have a epoll handler
static void addfd(int epfd, int fd, bool enable_et){
    // Create epool_event structure
    /*
    struct epoll_event {
        _uint32_t events;
        epoll_data_t data;
    }

    struct epoll_data_t {
        int fd;
        void *ptr;
        ...
    }
    */
    struct epoll_event ev;
    ev.data.fd = fd;
    ev.events = EPOLLIN;
    if(enable_et)
        ev.events |= EPOLLET;
    epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
    cout << "fd " << fd << " added to epoll!" << endl;
}

static void err_die(const char *str){
    perror(str);
    exit(EXIT_FAILURE);
}

size_t/* Write "n" bytes to a descriptor. */
writen(int fd, const void *vptr, size_t n)
{
        size_t nleft;
        ssize_t nwritten;
        const char*ptr;

        ptr = vptr;
        nleft = n;
        while (nleft > 0) {
            if ( (nwritten = write(fd, ptr, nleft)) <= 0 ) {
                if (nwritten < 0 && errno == EINTR)
                    nwritten = 0;/* and call write() again */
                else
                    return(-1);/* error */

            }

            nleft -= nwritten;
            ptr   += nwritten;

            }
            return(n);
}
/* end writen */


ssize_t/* Read "n" bytes from a descriptor. */
readn(int fd, void *vptr, size_t n)
{
    size_t  nleft;
    ssize_t nread;
    char    *ptr;

    ptr = vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = read(fd, ptr, nleft)) < 0 ) {
            if (errno == EINTR)
                nread = 0;/* and call read() again */
            else
                return(-1);
        } else if (nread == 0)
            break;/* EOF */

        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft);/* return >= 0 */
}
/* end readn */


static int read_cnt;
static char * read_ptr;
static char read_buf[MAXLINE];

static ssize_t
my_read(int fd, char *ptr)
{

    if (read_cnt <= 0) {
again:
    if ( (read_cnt = read(fd, read_buf, sizeof(read_buf))) < 0 ) {
        if (errno == EINTR)
            goto again;
        return(-1);
    } else if (read_cnt == 0)
        return(0);
        read_ptr = read_buf;
    }

    read_cnt--;
    *ptr = *read_ptr++;
    return(1);
}

ssize_t
readline(int fd, void *vptr, size_t maxlen)
{
    ssize_t n, rc;
    char c, *ptr;

    ptr = vptr;
    for (n = 1; n < maxlen; n++) {
        if ( (rc = my_read(fd, &c)) == 1 ) {
            *ptr++ = c;
            if (c == '\n')
                break;/* newline is stored, like fgets() */

        } else if (rc == 0) {
            *ptr = 0;
            return(n - 1);/* EOF, n - 1 bytes were read */

        } else
            return(-1);/* error, errno set by read() */
    }
    *ptr = 0;/* null terminate like fgets() */
    return(n);

}

#endif  //CHATROOM_COMMON_H
