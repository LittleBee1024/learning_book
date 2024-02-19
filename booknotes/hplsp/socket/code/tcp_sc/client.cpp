#include <stdio.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
   if (argc <= 2)
   {
      printf("usage: %s ip_address port_number\n", basename(argv[0]));
      return -1;
   }

   const char *ip = argv[1];
   int port = atoi(argv[2]);
   printf("[Client] Connect to server %s:%d\n", ip, port);

   sockaddr_in server_address;
   bzero(&server_address, sizeof(server_address));
   server_address.sin_family = AF_INET;
   inet_pton(AF_INET, ip, &server_address.sin_addr);
   server_address.sin_port = htons(port);

   int sock = socket(AF_INET, SOCK_STREAM, 0);
   assert(sock >= 0);

   int rc = connect(sock, (struct sockaddr *)&server_address, sizeof(server_address));
   assert(rc != -1);
   printf("[Client] Connected socket %d to the server\n", sock);

   char buffer[] = "Hello World";
   printf("[Client] Send '%s' to server\n", buffer);
   send(sock, buffer, sizeof(buffer), 0);

   printf("[Client] Close the socket %d\n", sock);
   close(sock);
   return 0;
}
