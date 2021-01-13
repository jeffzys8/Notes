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

#define BUFFER_SIZE 1024
static const char *STATUS_LINE[2] = {"200 OK", "500 Internal server error"};
static const char *HTTP_VERSION = "HTTP/1.1";

int main(int argc, char *argv[])
{
    if (argc <= 3)
    {
        printf("usage: %s ip port filename\n", argv[0]);
        return 1;
    }
    const char *ip = argv[1]; /* 对于const一直迷迷糊糊 */
    int port = atoi(argv[2]);
    const char *filename = argv[3];

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
        char header_buf[BUFFER_SIZE];
        bzero(header_buf, sizeof(header_buf));
        int header_len = 0;

        char *file_buf; /* 后面动态分配 */
        struct stat file_stat;
        bool file_valid = true;

        if (stat(filename, &file_stat) != 0 || /* 文件不存在 */
            S_ISDIR(file_stat.st_mode) ||      /* 是个目录 */
            !(file_stat.st_mode & S_IROTH))    /* others 无执行权限 */
        {
            file_valid = false;
        }
        else
        {
            int file_fd = open(filename, O_RDONLY);
            file_buf = new char[file_stat.st_size + 1]; /* 结尾有个'\0'? */
            bzero(file_buf, file_stat.st_size + 1);
            if (read(file_fd, file_buf, file_stat.st_size + 1) < 0) /* 读文件失败 */
            {
                file_valid = false;
            }
        }

        /* 发送response部分 */
        if (file_valid)
        {
            /* TODO: 这部分代码应该是能进一步优化的，存在冗余 */
            header_len += snprintf(header_buf, BUFFER_SIZE - 1, "%s %s \r\n", HTTP_VERSION, STATUS_LINE[0]);
            header_len += snprintf(header_buf + header_len, BUFFER_SIZE - header_len - 1,"Content-Length: %ld\r\n", file_stat.st_size);
            header_len += snprintf(header_buf + header_len, BUFFER_SIZE - header_len - 1, "\r\n");
            iovec iv[2];
            iv[0].iov_base = header_buf;
            iv[0].iov_len = header_len; /* or strlen(header_buf) */
            iv[1].iov_base = file_buf;
            iv[1].iov_len = file_stat.st_size;
            ret = writev(connfd, iv, 2); /* 两段空间使用writev */
            /* 测试代码 */
            printf("sizeof(iv): %lu\n", sizeof(iv));
            /* 测试代码结束 */   
        }
        else
        {
            header_len += snprintf(header_buf, BUFFER_SIZE - 1, "%s %s \r\n", HTTP_VERSION, STATUS_LINE[1]);
            header_len += snprintf(header_buf + header_len, BUFFER_SIZE - header_len - 1, "\r\n");
            send(connfd, header_buf, header_len, 0); /* or strlen(header_buf) */
        }

        close(connfd);
        delete[] file_buf; /* TODO: 如果是野指针会不会出错? */
    }

    close(sock);
    return 0;
}