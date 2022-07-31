#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 65537

int main(int argc, char *argv[])
{
   if (argc <= 2)
   {
      printf("usage: %s ip_address port_number\n", basename(argv[0]));
      return -1;
   }

   const char *ip = argv[1];
   int port = atoi(argv[2]);
   printf("[Server] Start server at %s:%d\n", ip, port);

   struct sockaddr_in address;
   bzero(&address, sizeof(address));
   address.sin_family = AF_INET;
   inet_pton(AF_INET, ip, &address.sin_addr);
   address.sin_port = htons(port);

   int sock = socket(AF_INET, SOCK_STREAM, 0);
   assert(sock >= 0);

   int rc = bind(sock, (struct sockaddr *)&address, sizeof(address));
   assert(rc != -1);

   rc = listen(sock, 5);
   assert(rc != -1);

   struct sockaddr_in client;
   socklen_t client_len = sizeof(client);
   printf("[Server] Waiting for connection...\n");
   int connfd = accept(sock, (struct sockaddr *)&client, &client_len);
   assert(connfd > 0);
   printf("[Server] Connection %d is created\n", connfd);

   char buffer[BUFFER_SIZE];
   memset(buffer, '\0', BUFFER_SIZE);
   printf("[Server] Dump data from client...\n");
   while (1)
   {
      int n_bytes = read(connfd, buffer, BUFFER_SIZE - 1);
      if (n_bytes == 0)
      {
         printf("\n[Server] Remote client socket was closed\n");
         break;
      }
      for (int i = 0; i < n_bytes; i++)
      {
         printf("%c", buffer[i]);
      }
   }

   printf("[Server] Close connection socket %d and listen socket %d\n", connfd, sock);
   close(connfd);
   close(sock);
   return 0;
}
