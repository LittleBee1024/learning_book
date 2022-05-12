#include <stdio.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <assert.h>

#define TEST_HOSTNAME "www.baidu.com"

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
}

void test_gethostbyname()
{
   printf("[gethostbyname]: %s\n", TEST_HOSTNAME);

   hostent *h = gethostbyname(TEST_HOSTNAME);

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

int main()
{
   test_getaddrinfo();
   test_gethostbyname();

   return 0;
}
