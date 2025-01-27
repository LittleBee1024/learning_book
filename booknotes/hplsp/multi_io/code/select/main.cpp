// https://www.ibm.com/docs/en/i/7.1?topic=designs-example-nonblocking-io-select
#include <sys/select.h>
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
      memcpy(&read_fds, &read_fds_copy, sizeof(read_fds_copy));
      timeout.tv_sec = 60; // seconds

      printf("[Server] Waiting on select()...\n");
      rc = select(max_fd + 1, &read_fds, nullptr /*write_fds*/, nullptr /*exception_fds*/, &timeout);
      assert(rc >= 0);
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
               assert(conn_fd > 0);
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
               assert(n_bytes >= 0);
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
               rc = send(i, buf, n_bytes, 0);
               assert(rc != -1);
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
