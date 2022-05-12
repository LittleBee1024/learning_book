# 套接字基础

> [《Linux高性能服务器编程》 - 游双 ](https://1drv.ms/b/s!AkcJSyT7tq80c1DmkdcxK7oScvQ)的第5章，以及[《UNIX网络编程卷1》](https://1drv.ms/b/s!AkcJSyT7tq80dP1Vghbg7qb9uts)的第3、7、11章的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/sock_base/code)中找到

## 套接字地址
大多数套接字都需要一个指向套接字地址结构的指针作为参数。每个协议都定义了它自己的套接字地址结构：

协议族 | 地址族 | 结构体 | 地址含义和长度
--- | --- | --- | ---
PF_UNIX | AF_UNIX | `sockaddr_un` | UNIX本地域协议，文件的路径名，长度可达108字节
PF_INET | AF_INET | `sockaddr_in` | TCP/IPv4协议，16bit端口号和32bit IPv4地址，共6字节
PF_INET6 | AF_INET6 | `sockaddr_in6` | TCP/IPv6协议，16bit端口号，32bit流标识，128bit IPv6地址，32bit范围ID，共26字节

### 通用socket地址
```cpp
#include <bits/socket.h>

// 通用套接字地址类型
//  sa_family - 地址族类型
//  套接字函数需要支持不同协议族的套接字地址结构，因此定义了通用地址结构`sockaddr`以接受任何协议的指针参数。
//  例如，`int bind(int, struct sockaddr *, socklen_t)`函数的第二个参数接受IPv4协议地址时，需要如下强制转换：
//      struct sockaddr_in serv;
//      bind(sockfd, (struct sockaddr *)&serv, sizeof(serv));
struct sockaddr
{
    sa_family_t sa_family;
    char sa_data[14];
}

// 14字节的`sockaddr.sa_data`不足以容纳多数协议族的地址值，因此Linux定义了`sockaddr_storage`用于真正的存储空间
struct sockaddr_storage
{
    sa_family_t sa_family;
    unsigned long int __ss_align;
    char __ss_padding[128-sizeof(__ss_align)];
}
```

不同套接字地址结构的比较如下：

![sockaddr](./images/sockaddr.png)

### IPv4 socket地址
TCP/IPv4协议族的socket地址结构`sockaddr_in`如下：
```cpp
struct sockaddr_in
{
    sa_family_t sin_family; // 地址族：AF_INET
    u_int16_t sin_port;     // 端口号，要用网络字节序(大端)表示
    struct in_addr sin_addr;// IPv4地址结构体
};

struct in_addr
{
    u_int32_t s_addr;       // IPv4地址，要用网络字节序(大端)表示
};
```

人们习惯用字符串来表示IP地址，下面的函数可用于用点分十进制字符串表示的IPv4地址和用网络字节序整数表示的IPv4地址之间的转换：
```cpp
#include <arpa/inet.h>

// 将用点分十进制字符串的IPv4地址转换为网络字节序整数表示的地址
in_addr_t inet_addr(const char* strptr);

// 和inet_addr功能一样，但将结果存于inp指向的结构中，成功返回1
int inet_aton(const char* cp, struct in_addr* inp);

// 将网络字节序整数表示的IPv4地址转化为用点分十进制字符串的IPv4地址
char* inet_ntoa(struct in_addr in);

// inet_aton的通用版本，也可用于IPv6的地址转换
int inet_pton(int af, const char* src, void* dst);

// inet_ntoa的通用版本，也可用于IPv6的地址转换，成功返回指向dst的指针
const char* inet_ntop(int af, const void* src, char* dst, socklen_t cnt);
```

[例子"addr_conv"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/sock_base/code/addr_conv)利用上面的函数，完成了IPv4字符串地址和网络字节地址的转换：
```cpp
#define IP_STR "192.0.2.33"

int main()
{
   struct sockaddr_in sa;
   char str[INET_ADDRSTRLEN];

   memset(&sa, 0, sizeof(sa));
   inet_pton(AF_INET, IP_STR, &(sa.sin_addr));
   inet_ntop(AF_INET, &(sa.sin_addr), str, INET_ADDRSTRLEN);
   printf("[inet_pton/inet_ntop] convert between %d and %s\n", sa.sin_addr.s_addr, str);

   memset(&sa, 0, sizeof(sa));
   inet_aton(IP_STR, &(sa.sin_addr));
   // ip指向inet_ntoa内部的一个静态变量，因此inet_ntoa不可重入
   char* ip = inet_ntoa(sa.sin_addr);
   printf("[inet_addr/inet_aton/inet_ntoa] convert between %d(%d) and %s\n", inet_addr(IP_STR), sa.sin_addr.s_addr, ip);

   return 0;
}
```
```bash
> ./main
[inet_pton/inet_ntop] convert between 553779392 and 192.0.2.33
[inet_addr/inet_aton/inet_ntoa] convert between 553779392(553779392) and 192.0.2.33
```

### 名字与地址的转换

```cpp
#include <netdb.h>

// 根据主机名/服务名，获得主机或者服务的地址信息，是对`gethostbyname`和`getservbyname`的封装
//  hints - 可设置ai_flags, ai_family, ai_socktype, ai_protocol，其他必须为NULL
int getaddrinfo(const char *restrict node,
    const char *restrict service,
    const struct addrinfo *restrict hints,
    struct addrinfo **restrict res);
// 销毁`getaddrinfo`返回的res
void freeaddrinfo(struct addrinfo *res);

// 根据主机/服务地址信息，获得以字符串表示的主机/服务名，是对`gethostbyaddr`和`getservbyport`的封装
int getnameinfo(const struct sockaddr *restrict addr, socklen_t addrlen,
    char *restrict host, socklen_t hostlen,
    char *restrict serv, socklen_t servlen, int flags);

struct addrinfo
{
    int ai_flags;       // 控制`hints`行为，如配置为`AI_CANONNAME`，会返回主机名
    int ai_family;      // 地址簇，AF_INET/AF_INET5/AF_UNIX
    int ai_socktype;    // 服务类型，SOCK_STREAM/SOCK_DGRAM
    int ai_protocol;    // 网络协议，通常为零，需要和`ai_family`的值配套
    socklen_t ai_addrlen;
    char* ai_canonname; // 主机的别名
    struct sockaddr* ai_addr;
    struct addrinfo* ai_next;
};
```
