#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include "ddpg.hpp"

int ddpg_connect()
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
        ddpg_conn = accept(sockfd, NULL, NULL);
        if (ddpg_conn == -1) {
                perror("accept");
                return -1;
        }
        shutdown(sockfd, 2);
        close(sockfd);
        unlink(sock_path);
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
