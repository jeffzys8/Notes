#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

static bool stop = false;
static void handle_term(int sig)
{
    printf("程序关闭\n");
    stop = true;
}

int main(int argc, char *argv[])
{
    signal(SIGTERM, handle_term);
    if (argc <= 3)
    {
        printf("usage: %s, %s\n", argv[0], basename(argv[0]));
        return 1;
    }
    const char *ip = argv[1]; /* 对于const一直迷迷糊糊 */

    int port = atoi(argv[2]);
    int backlog = atoi(argv[3]);

    int sock = socket(PF_INET, SOCK_STREAM, 0);
    assert(sock >= 0);
    printf("sock: %d\n", sock); /* 多次运行返回同一个socket, 说明程序结束被自动回收了 */

    /* 测试各个地址struct的占用空间大小 */
    printf("%lu, %lu, %lu, %lu\n", sizeof(sockaddr_in), sizeof(sockaddr_in6), sizeof(sockaddr_storage), sizeof(sockaddr));

    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    // printf("addr: %d\n", address.sin_addr.s_addr);
    address.sin_port = htons(port);
    /* origin port = 8888 = b0010 0010 1011 1000 */
    printf("port: %d\n", address.sin_port); /* expected to be 1011 1000 0010 0010 */

    int ret = bind(sock, (sockaddr*)&address, sizeof(address));
    assert(ret != -1);

    ret = listen(sock, backlog);
    assert(ret != -1);

    while(!stop)
    {
        sleep(1);
    }

    /* wsl的netstat查不到任何地址，这应该是遇到的第一个比较大的坑 */
    /* telnet quit以后一直处于FIN_WAIT2 & CLOSE_WAIT 状态，应该是因为服务端没有close */
    close(sock);
    return 0;
}