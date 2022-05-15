# IO复用

> [《Linux高性能服务器编程》 - 游双 ](https://1drv.ms/b/s!AkcJSyT7tq80c1DmkdcxK7oScvQ)的第9章，[《UNIX网络编程卷1》 - 第三版 ](https://1drv.ms/b/s!AkcJSyT7tq80dP1Vghbg7qb9uts)的第6章，以及[《小林coding - 网络系统》](https://xiaolincoding.com/os)的读书笔记，本文中的所有代码可在[GitHub仓库](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/multi_io/code)中找到

## IO复用函数

![io_multiplex](./images/io_multiplex.png)

Linux提供了`select()`，`poll()`，`epoll()`三个函数，用于监控文件描述符上发生的事件，实现IO复用。其中，`select()`，`poll()`是类UNIX系统都提供的，而`epoll()`是Linux独有的。

下面通过三个例子，分别介绍这三个函数的使用方法。这三个例子可同时处理新的客户端网络连接和多个客户端输入，并将客户端传来的字符串转换成大写后回传给客户端：

![io_multiplex_examples](./images/io_multiplex_examples.png)

### select

```cpp
#include <sys/select.h>

// 监听三组文件描述符上发生的事件
//  nfds - 被监听的文件描述符的总数，通常是select监听的所有文件描述符的最大值加1
//  readfds, writefds, exceptfds - 记录要监听的三种类型的文件描述符
int select(int nfds, fd_set* readfds, fd_set* writefds, fd_set* exceptfds, struct timeval* timeout);
```

* 文件描述符就绪条件
    * socket可读
        * 接收缓存区的字节数大于或等于`SO_RCVLOWAT`
        * socket通信的对方关闭连接，此时对该socket的读操作将返回0
        * 监听socket上有新的连接请求
        * socket上有未处理的错误
    * socket可写
        * 发送缓存区的字节数大于或等于`SO_SNDLOWAT`
        * socket的写操作被关闭
        * socket使用非阻塞connect连接成功或者失败(超时)之后
        * socket上有未处理的错误
    * socket异常
        * socket上接收到外带数据，发送的时候带上`MSG_OOB`标志

[例子"select"](https://github.com/LittleBee1024/learning_book/tree/main/docs/booknotes/hplsp/multi_io/code/select)利用`select()`函数，完成了对客户端连接和客户端输入的监听。需要注意的是，`select()`调用返回后，用户需要遍历所有文件描述符，找到发生了事件的文件描述符后，再进行处理。并且，每次调用`select()`前都要重新设置文件描述符，因为内核会修改传入的文件描述符。

```cpp title="server.cpp" hl_lines="30 37 40"
int main(int argc, char *argv[])
{
   ...
   int listen_fd = socket(PF_INET, SOCK_STREAM, 0);

   int on = 1;
   setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
   // 配置监听socket为非阻塞
   ioctl(listen_fd, FIONBIO, (char *)&on);

   bind(listen_fd, (struct sockaddr *)&address, sizeof(address));
   listen(listen_fd, 5);

   fd_set read_fds_copy;
   FD_ZERO(&read_fds_copy);
   FD_SET(listen_fd, &read_fds_copy);

   int max_fd = listen_fd;
   struct timeval timeout;
   char buf[1024];
   fd_set read_fds;
   FD_ZERO(&read_fds);
   while (true)
   {
      // 每次调用select前都要重新在read_fds中设置文件描述符，因为事件发生之后，文件描述符将被内核修改
      memcpy(&read_fds, &read_fds_copy, sizeof(read_fds_copy));
      timeout.tv_sec = 60; // seconds

      printf("[Server] Waiting on select()...\n");
      select(max_fd + 1, &read_fds, nullptr /*write_fds*/, nullptr /*exception_fds*/, &timeout);
      if (rc == 0)
      {
         printf("[Server] select() timeout, end program\n");
         break;
      }

      for (int i = 0; i <= max_fd; ++i)
      {
         // file descriptor `i` has read event
         if (FD_ISSET(i, &read_fds))
         {
            if (i == listen_fd)
            {
               printf("[Server] Listening socket is readable\n");

               struct sockaddr_in client;
               socklen_t client_addrlength = sizeof(client);
               int conn_fd = accept(listen_fd, (struct sockaddr *)&client, &client_addrlength);
               printf("[Server] New incoming connection %d, ip: %s, port: %d\n",
                      conn_fd, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

               FD_SET(conn_fd, &read_fds_copy);
               if (conn_fd > max_fd)
                  max_fd = conn_fd;
            }
            else
            {
               printf("[Server] Connection socket %d is readable\n", i);

               memset(buf, '\0', sizeof(buf));
               int n_bytes = recv(i, buf, sizeof(buf) - 1, 0);
               if (n_bytes == 0)
               {
                  printf("[Server] Remote client was closed, so close connection %d\n", i);
                  close(i);
                  FD_CLR(i, &read_fds_copy);
                  if (i == max_fd)
                  {
                     while (FD_ISSET(max_fd, &read_fds) == false)
                        max_fd -= 1;
                  }
                  break;
               }

               printf("[Server] Get %d bytes from connection %d : %s\n", n_bytes, i, buf);
               for (int j = 0; j < n_bytes; j++)
                  buf[j] = toupper(buf[j]);
               send(i, buf, n_bytes, 0);
            }
         }
      }
   }

   for (int i = 0; i <= max_fd; ++i)
   {
      if (FD_ISSET(i, &read_fds_copy))
         close(i);
   }

   return 0;
}
```

启动服务后，通过`nc`命令，连接两个客户端到服务器，分别传送"hello"和"world"字符串给服务器，观察服务器是否回传大写的"HELLO"和"WORLD"。再分别退出客户端连接，观察服务端打印的信息。最后等待60秒后，由于服务端没有收到任何信息，超时退出。

=== "Server"

    ```bash
    > ./main 127.0.0.1 1234
    [Server] Waiting on select()...
    [Server] Listening socket is readable
    [Server] New incoming connection 4, ip: 127.0.0.1, port: 56274
    [Server] Waiting on select()...
    [Server] Listening socket is readable
    [Server] New incoming connection 5, ip: 127.0.0.1, port: 56280
    [Server] Waiting on select()...
    [Server] Connection socket 4 is readable
    [Server] Get 6 bytes from connection 4 : hello

    [Server] Waiting on select()...
    [Server] Connection socket 5 is readable
    [Server] Get 6 bytes from connection 5 : world

    [Server] Waiting on select()...
    [Server] Connection socket 4 is readable
    [Server] Remote client was closed, so close connection 4
    [Server] Waiting on select()...
    [Server] Connection socket 5 is readable
    [Server] Remote client was closed, so close connection 5
    [Server] Waiting on select()...
    [Server] Waiting on select()...
    [Server] select() timeout, end program
    ```

=== "Client"

    ```bash
    # 客户端1连接服务器，发送"hello"后`Ctrl+D`退出
    > nc -q 1 127.0.0.1 1234
    hello
    HELLO
    ```
    ```bash
    # 客户端1连接服务器，发送"world"后`Ctrl+D`退出
    > nc -q 1 127.0.0.1 1234
    world
    WORLD
    ```

### poll

### epoll

