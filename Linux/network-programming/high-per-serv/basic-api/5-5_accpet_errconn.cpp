#include <stdio.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <errno.h>
#include <string.h>

int main(int argc, char *argv[])
{
    if (argc <= 2)
    {
        printf("usage: %s, %s\n", argv[0], basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1]; /* 对于const一直迷迷糊糊 */
    int port = atoi(argv[2]);

    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int reuse = 1;

    /* 试验SO_REUSEADDR重用TIME_WAIT地址 */
    // setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    int ret = bind(sock, (sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5); // TODO: backlog 5-3代码还没跑
    assert(ret != -1);

    /* stop 20 sec to wait for mannual disconnection operation finish */
    // sleep(20); /* sleep 的代码可以研究一下，如何使用SIGALRM并避免用户信号的影响 */

    sockaddr_in client;
    socklen_t client_addrlen = sizeof(client);
    for (int i = 0; i < 2; ++i) /* 试验能否在TIME_WAIT状态下重新进行一个同IP同端口的连接 */
    {
        int connfd = accept(sock, (sockaddr *)&client, &client_addrlen);
        if (connfd < 0)
        {
            printf("errno is: %d\n", errno);
        }
        else
        {
            char remote[INET_ADDRSTRLEN];
            printf("connected with ip: %s and port:%d\n", inet_ntop(AF_INET, &client.sin_addr, remote, INET_ADDRSTRLEN), ntohs(client.sin_port));
            close(connfd);
        }
    }
    close(sock);
    return 0;
}