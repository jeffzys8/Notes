#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>

int main()
{
    /* `inet_addr` */
    printf("example for `inet_addr`:\n");
    uint32_t addr1 = inet_addr("127.0.0.1");
    uint32_t addr2 = inet_addr("192.168.1.1");
    printf("%d %d\n", addr1, addr2);
    /* illustrate little endian again */
    printf("little endian illustration\n");
    printf("addr1: low mem -----> high mem\n");    
    while (addr1 != 0)
    {
        printf("%d", addr1 % 2);
        addr1 /= 2;
    }
    printf("\n");
    printf("----------------------------------\n");

    /* `inet_aton` */
    printf("example for `inet_aton`:\n");
    struct in_addr dst1, dst2;
    printf("uninitialized: %d, %d\n", dst1.s_addr, dst2.s_addr);
    inet_aton("127.0.0.1", &dst1);
    inet_aton("192.168.1.1", &dst2);
    printf("after: %d, %d\n", dst1.s_addr, dst2.s_addr);
    printf("after(first 8bit): %d, %d\n", dst1.s_addr % (1<<8), dst2.s_addr % (1<<8));
    printf("after(last 8bit): %d, %d\n", (dst1.s_addr >>24) % (1<<8), (dst2.s_addr >>24) % (1<<8));
    printf("----------------------------------\n");
    
    /* `inet_ntoa` */
    printf("example for `inet_ntoa`:\n"); /* reuse dst1, dst2 */
    printf("dst1: %s\n", inet_ntoa(dst1));
    printf("dst2: %s\n", inet_ntoa(dst2));
    /* illustrate nonreentrancy of `inet_ntoa` */
    printf("ptr: %p, %p\n", inet_ntoa(dst1), inet_ntoa(dst2));
    printf("----------------------------------\n");

    /* `inet_pton` with IPv4*/
    printf("example for `inet_pton` with IPv4:\n"); /* reuse dst1, dst2 */
    inet_pton(AF_INET, "159.148.22.12", &dst1.s_addr);
    inet_pton(AF_INET, "49.57.0.1", &dst2.s_addr);
    printf("after: %d, %d\n", dst1.s_addr, dst2.s_addr);
    printf("----------------------------------\n");
    
    /* `inet_ntop` with IPv4*/
    printf("example for `inet_ntop` with IPv4:\n"); /* reuse dst1, dst2 */
    unsigned char v4str1[20], v4str2[20];
    memset(v4str1, 0, sizeof(v4str1));
    memset(v4str2, 0, sizeof(v4str1));
    printf("str set to 0: %s, %s\n", v4str1, v4str2);
    const char* a = inet_ntop(AF_INET, &dst1, v4str1, sizeof(v4str1));
    const char* b = inet_ntop(AF_INET, &dst2, v4str2, sizeof(v4str2));
    printf("after: %s, %s, %s, %s\n", v4str1, v4str2, a, b);
    printf("is return and buf same addr: %p vs %p, %p vs %p\n", a, v4str1, b, v4str2);
    printf("----------------------------------\n");

    /* `inet_pton` with AF_INET6*/
    printf("example for `inet_pton` in IPv6:\n");
    struct in6_addr dstv6_1, dstv6_2;
    int r1 = inet_pton(AF_INET6, "0001:0002::000e", &dstv6_1);
    int r2 = inet_pton(AF_INET6, "1000:2000::f000", &dstv6_2);
    printf("success: %d, %d\n", r1, r2);
    printf("dstv6_1(0001:0002::000e): ");
    for (int i = 0; i < sizeof(dstv6_1.__in6_u.__u6_addr8); i++) {
        printf("%d ", dstv6_1.__in6_u.__u6_addr8[i]);
    }
    printf("\n");
    printf("dstv6_2(1000:2000::f000): ");
    for (int i = 0; i < sizeof(dstv6_2.__in6_u.__u6_addr8); i++) {
        printf("%d ", dstv6_2.__in6_u.__u6_addr8[i]);
    }
    printf("\n");
    printf("----------------------------------\n");

    /* `inet_ntop` with AF_INET6*/
    printf("example for `inet_ntop` in IPv6:\n"); /* reuse dstv6_1, dstv6_2 */
    unsigned char v6str1[128], v6str2[128];
    memset(v6str1, 0, sizeof(v6str1));
    memset(v6str2, 0, sizeof(v6str2));
    inet_ntop(AF_INET6, &dstv6_1, v6str1, sizeof(v6str1));
    inet_ntop(AF_INET6, &dstv6_2, v6str2, sizeof(v6str2));
    printf("result: %s, %s\n", v6str1, v6str2);
    return 0;
}