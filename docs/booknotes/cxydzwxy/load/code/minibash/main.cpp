#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <iostream>
#include <string>

int main()
{
   while (1)
   {
      printf("minibash$ ");
      std::string cmd;
      std::getline(std::cin, cmd);
      pid_t pid = fork();
      if (pid == 0)
      {
         if (execlp("/bin/sh", "sh", "-c", cmd.c_str(), (char *)NULL) < 0)
         {
            printf("exec error\n");
         }
      }
      else if (pid > 0)
      {
         int status;
         waitpid(pid, &status, 0);
         if (cmd == "exit")
            return 0;
      }
      else
      {
         printf("fork error %d\n", pid);
      }
   }
   return 0;
}

