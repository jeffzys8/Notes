#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/uio.h>
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
        printf("usage: %s ip port\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1];
    int port = atoi(argv[2]);

    sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);

    int reuse = 1;
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)); /* 受够了..reuse一下 */

    int ret = bind(sock, (sockaddr *)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, 5);
    assert(ret != -1);

    sockaddr_in client;
    socklen_t client_addrlen = sizeof(client);

    int connfd = accept(sock, (sockaddr *)&client, &client_addrlen);
    if (connfd < 0)
    {
        printf("errno is: %d\n", errno);
    }
    else
    {
        int pipefd[2];
        assert(pipe(pipefd) != -1);
        // loff_t off = 2; /* 没搞明白offset怎么用 */
        assert(splice(connfd, NULL, pipefd[1], NULL, 32768, SPLICE_F_MORE) != -1);
        assert(splice(pipefd[0], NULL, connfd, NULL, 32768, SPLICE_F_MORE) != -1);
        close(connfd);
    }
    close(sock);
    return 0;
}