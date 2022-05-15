#include <sys/epoll.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <sys/ioctl.h>

#define MAX_EVENTS 5

int main(int argc, char *argv[])
{
   if (argc <= 2)
   {
      printf("usage: %s ip_address port_number\n", basename(argv[0]));
      return 1;
   }

   const char *ip = argv[1];
   int port = atoi(argv[2]);

   struct sockaddr_in address;
   bzero(&address, sizeof(address));
   address.sin_family = AF_INET;
   inet_pton(AF_INET, ip, &address.sin_addr);
   address.sin_port = htons(port);

   int listen_fd = socket(PF_INET, SOCK_STREAM, 0);
   assert(listen_fd >= 0);

   int on = 1;
   int rc = setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
   assert(rc != -1);
   rc = ioctl(listen_fd, FIONBIO, (char *)&on);
   assert(rc != -1);

   rc = bind(listen_fd, (struct sockaddr *)&address, sizeof(address));
   assert(rc != -1);

   rc = listen(listen_fd, 5);
   assert(rc != -1);

   int epoll_fd = epoll_create1(0);
   assert(epoll_fd > 0);
   struct epoll_event event, events[MAX_EVENTS];
   event.events = EPOLLIN;
   event.data.fd = listen_fd;
   rc = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);
   assert(rc != -1);

   int timeout = 60 * 1000; // milliseconds
   char buf[1024];
   while (true)
   {
      printf("Waiting on epoll()...\n");
      int event_count = epoll_wait(epoll_fd, events, MAX_EVENTS, timeout);
      assert(event_count >= 0);
      if (event_count == 0)
      {
         printf("[Server] poll() timeout, end program\n");
         break;
      }

      for (int i = 0; i < event_count; ++i)
      {
         printf("[Server] Event 0x%x of %d events from file descriptor %d\n",
            events[i].events, event_count, events[i].data.fd);

         if ((events[i].data.fd == listen_fd) && (events[i].events & EPOLLIN))
         {
            printf("[Server] Listening socket is readable\n");

            struct sockaddr_in client;
            socklen_t client_addrlength = sizeof(client);
            int conn_fd = accept(listen_fd, (struct sockaddr *)&client, &client_addrlength);
            assert(conn_fd > 0);
            printf("[Server] New incoming connection %d, ip: %s, port: %d\n",
                   conn_fd, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

            event.data.fd = conn_fd;
            event.events = EPOLLIN;
            rc = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &event);
            assert(rc != -1);
         }
         else if (events[i].events & EPOLLIN)
         {
            printf("[Server] Connection socket %d is readable\n", events[i].data.fd);

            memset(buf, '\0', sizeof(buf));
            int n_bytes = recv(events[i].data.fd, buf, sizeof(buf) - 1, 0);
            assert(n_bytes >= 0);
            if (n_bytes == 0)
            {
               printf("[Server] Remote client was closed, so close connection %d\n", events[i].data.fd);
               rc = epoll_ctl(epoll_fd, EPOLL_CTL_DEL, events[i].data.fd, nullptr);
               assert(rc != -1);
               close(events[i].data.fd);
               continue;
            }

            printf("[Server] Get %d bytes from connection %d : %s\n", n_bytes, events[i].data.fd, buf);
            for (int j = 0; j < n_bytes; j++)
               buf[j] = toupper(buf[j]);
            rc = send(events[i].data.fd, buf, n_bytes, 0);
            assert(rc != -1);
         }
         else
         {
            printf("[Server] Receive unexpected event 0x%x", events[i].data.fd);
            assert(0);
         }
      }
   }

   close(epoll_fd);
   // TODO: close connection file descriptor, which needs a list to manintain the open connection file desciptor
   close(listen_fd);

   return 0;
}
