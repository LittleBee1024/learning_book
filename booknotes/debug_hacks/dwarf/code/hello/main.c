#include <stdlib.h>

int main(int argc, const char **argv)
{
   int num = 1;
   int *p_num = malloc(sizeof(int));
   *p_num = 2;
   free(p_num);
   return 0;
}
