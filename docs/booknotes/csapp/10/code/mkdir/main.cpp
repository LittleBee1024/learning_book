#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include <stdlib.h>

int main()
{
   int rc = mkdir("dir1", 0777);
   assert(rc == 0);
   const char *path = "dir2/dir3";
   char cmd[100];
   sprintf(cmd, "mkdir -p %s", path);
   rc = system(cmd);
   assert(rc == 0);
   return 0;
}
