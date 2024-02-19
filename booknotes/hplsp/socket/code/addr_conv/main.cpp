#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>

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

