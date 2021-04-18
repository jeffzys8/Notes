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
        close(STDOUT_FILENO);
        int dup_sock = dup(connfd);
        printf("abcd\n");
        /* 
        如果要调用close(dup_sock), 由于STDOUT被关闭，类似重定向，
        printf行缓冲变成了全缓冲，因此要加上fflush才有输出 
        */
        fflush(stdout); 

        /* 
        经验证只close(connfd)确实不能将连接关闭，因为dup还复制了一份

        另外，仅调用close(connfd)而不调用close(dup_sock)竟然还会有输出，
        但我倾向于认为这是未定义的行为，依赖于程序结束时对printf缓冲区和网络连接的回收顺序，
        这里应该是程序结束时先刷新了缓冲区，而后关闭未关闭的网络连接，所以才能将数据发出，
        通过sleep和tcpdump抓包可以验证，确实是10s后才发出数据并立即关闭了连接。
        */
        close(connfd); 
        close(dup_sock);
        
        /* 暂停，用于验证上面关于close和printf缓冲区的理解 */
        sleep(10);

        // shutdown(connfd, SHUT_RDWR); /* shutdown可以直接关闭连接而不需要两次close */
    }
    close(sock);
    return 0;
}