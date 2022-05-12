#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>
#include <errno.h>
#include <stdlib.h>

#define TEST_HOSTNAME "www.baidu.com"
#define TEST_IP "142.250.194.100"

void test_getaddrinfo()
{
   printf("[getaddrinfo]: %s\n", TEST_HOSTNAME);

   struct addrinfo hints;
   struct addrinfo *result;

   memset(&hints, 0, sizeof(struct addrinfo));
   hints.ai_flags = AI_CANONNAME;
   hints.ai_family = AF_INET;
   hints.ai_socktype = SOCK_STREAM;

   int rc = getaddrinfo(TEST_HOSTNAME, NULL, &hints, &result);
   assert(rc == 0);

   printf("  Host name: %s\n", result->ai_canonname);
   printf("  Address type: %s\n", (result->ai_family == AF_INET) ? "AF_INET" : "Unknown");

   char str[INET_ADDRSTRLEN];
   printf("  Address(es): ");
   for (struct addrinfo *rp = result; rp != NULL; rp = rp->ai_next)
   {
      struct sockaddr_in *p_sockaddr = (struct sockaddr_in *)rp->ai_addr;
      inet_ntop(rp->ai_family, &(p_sockaddr->sin_addr), str, INET_ADDRSTRLEN);
      printf(" %s", str);
   }
   printf("\n");

   freeaddrinfo(result);
}

void test_getnameinfo()
{
   struct sockaddr_in sa;
   memset(&sa, 0, sizeof(sa));
   sa.sin_family = AF_INET;
   inet_aton(TEST_IP, &(sa.sin_addr));

   printf("[getnameinfo]: %s\n", inet_ntoa(sa.sin_addr));
   char host[NI_MAXHOST];
   int rc = getnameinfo((struct sockaddr *)&sa, sizeof(struct sockaddr), host, NI_MAXHOST, nullptr, 0, NI_NAMEREQD);
   if (rc != 0)
   {
      printf("[Error] %s. Please change TEST_IP.\n", gai_strerror(rc));
      exit(EXIT_FAILURE);
   }
   printf("  Host name: %s\n", host);
}

void _dump_hostent(hostent *h)
{
   printf("  Host name: %s\n", h->h_name);

   printf("  Alias(es): ");
   for (char **pp = h->h_aliases; *pp != NULL; pp++)
      printf(" %s", *pp);
   printf("\n");

   printf("  Address type: %s\n", (h->h_addrtype == AF_INET) ? "AF_INET" : "Unknown");

   char str[INET_ADDRSTRLEN];
   printf("  Address(es): ");
   for (char **pp = h->h_addr_list; *pp != NULL; pp++)
      printf(" %s", inet_ntop(h->h_addrtype, *pp, str, INET_ADDRSTRLEN));
   printf("\n");
}

void test_gethostbyname()
{
   printf("[gethostbyname]: %s\n", TEST_HOSTNAME);
   hostent *h = gethostbyname(TEST_HOSTNAME);
   _dump_hostent(h);
}

void test_gethostbyaddr()
{
   struct in_addr addr;
   inet_aton(TEST_IP, &(addr));
   printf("[gethostbyaddr]: %s\n", inet_ntoa(addr));
   hostent *h = gethostbyaddr((const char *)&addr, sizeof(addr), AF_INET);
   // If fails, please check "h_errno", maybe it is because an unknown host or a misconfigured DNS
   assert(h != NULL);
   _dump_hostent(h);
}

int main()
{
   test_getaddrinfo();
   test_getnameinfo();

   test_gethostbyname();
   test_gethostbyaddr();
   return 0;
}
