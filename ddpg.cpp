#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <fcntl.h>
#include "ddpg.hpp"

static int accept_with_timeout(int listenfd, int timeout_ms)
{
        int sockfd, res;
        fd_set readfds;
        struct timeval tv, *timeout = NULL;
        if (timeout_ms != -1) {
                tv.tv_sec = timeout_ms / 1000;
                tv.tv_usec = (timeout_ms % 1000) * 1000;
                timeout = &tv;
        }
        FD_ZERO(&readfds);
        FD_SET(listenfd, &readfds);
        res = select(listenfd + 1, &readfds, NULL, NULL, timeout);
        if (res == -1) {
                perror("select");
                return -1;
        }
        if (res == 0) {
                fprintf(stderr, "accept: connect timeout\n");
                return -1;
        }
        sockfd = accept(listenfd, NULL, NULL);
        if (sockfd == -1) {
                perror("accept");
                return -1;
        }
        return sockfd;
}

int ddpg_connect(int timeout)
{
        int sockfd, ddpg_conn, res;
        const char *sock_path = "./ddpg-bridge.sock";
        struct sockaddr_un addr;
        addr.sun_family = AF_UNIX;
        strncpy(addr.sun_path, sock_path, sizeof(addr.sun_path));
        sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
        if (sockfd == -1) {
                perror("socket");
                return -1;
        }
        unlink(sock_path);
        res = bind(sockfd, (struct sockaddr *)&addr, sizeof(addr));
        if (res == -1) {
                perror("bind");
                return -1;
        }
        res = listen(sockfd, 5);
        if (res == -1) {
                perror("listen");
                return -1;
        }
        ddpg_conn = accept_with_timeout(sockfd, timeout);
        shutdown(sockfd, 2);
        close(sockfd);
        return ddpg_conn;
}

void get_action(int ddpg_conn, ddpg_action *action_buf)
{
        int res = recv(ddpg_conn, action_buf, sizeof(ddpg_action), 0);
//        if (res == -1)
//                perror("recv");
}

void response_action(int ddpg_conn, ddpg_response *response_buf)
{
        int res = send(ddpg_conn, response_buf, sizeof(ddpg_response), 0);
//        if (res == -1)
//                perror("send");
}
