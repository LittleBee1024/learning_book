// https://www.ibm.com/docs/en/i/7.4?topic=designs-using-poll-instead-select
#include <poll.h>
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

#define MAX_POLL_FD 5

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

   struct pollfd fds[MAX_POLL_FD];
   memset(fds, 0, sizeof(fds));
   fds[0].fd = listen_fd;
   fds[0].events = POLLIN;
   int nfds = 1;
   int timeout = 60 * 1000; // milliseconds
   char buf[1024];
   while (true)
   {
      printf("Waiting on poll()...\n");
      rc = poll(fds, nfds, timeout);
      assert(rc >= 0);
      if (rc == 0)
      {
         printf("[Server] poll() timeout, end program\n");
         break;
      }

      int current_size = nfds;
      for (int i = 0; i < current_size; ++i)
      {
         //printf("[Server] fds[%d].fd = %d, fds[%d].revents = 0x%x\n", i, fds[i].fd, i, fds[i].revents);
         if (fds[i].revents == 0)
            continue;

         if ((fds[i].fd == listen_fd) && (fds[i].revents & POLLIN))
         {
            printf("[Server] Listening socket is readable\n");

            struct sockaddr_in client;
            socklen_t client_addrlength = sizeof(client);
            int conn_fd = accept(listen_fd, (struct sockaddr *)&client, &client_addrlength);
            assert(conn_fd > 0);
            printf("[Server] New incoming connection %d, ip: %s, port: %d\n",
                   conn_fd, inet_ntoa(client.sin_addr), ntohs(client.sin_port));

            fds[nfds].fd = conn_fd;
            fds[nfds].events = POLLIN;
            nfds++;
         }
         else if (fds[i].revents & POLLIN)
         {
            printf("[Server] Connection socket %d is readable\n", fds[i].fd);

            memset(buf, '\0', sizeof(buf));
            int n_bytes = recv(fds[i].fd, buf, sizeof(buf) - 1, 0);
            assert(n_bytes >= 0);
            if (n_bytes == 0)
            {
               printf("[Server] Remote client was closed, so close connection %d\n", fds[i].fd);
               close(fds[i].fd);
               fds[i] = fds[nfds - 1];
               nfds--;
               break;
            }

            printf("[Server] Get %d bytes from connection %d : %s\n", n_bytes, fds[i].fd, buf);
            for (int j = 0; j < n_bytes; j++)
               buf[j] = toupper(buf[j]);
            rc = send(fds[i].fd, buf, n_bytes, 0);
            assert(rc != -1);
         }
         else
         {
            printf("[Server] Receive unexpected event 0x%x", fds[i].revents);
            assert(0);
         }
      }
   }

   for (int i = 0; i < nfds; i++)
   {
      if (fds[i].fd > 0)
         close(fds[i].fd);
   }

   return 0;
}
