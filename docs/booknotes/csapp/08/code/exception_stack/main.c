#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <execinfo.h>
#include <string.h>

typedef void (*SigactionHandler)(int, siginfo_t *, void *);
void CreateSignalHandler(int s, SigactionHandler handler)
{
   struct sigaction sa
   {
   };
   sa.sa_flags = SA_SIGINFO | SA_RESTART;
   sigemptyset(&sa.sa_mask);
   sa.sa_sigaction = handler;
   int rc = sigaction(s, &sa, NULL);
   if (rc != 0 && errno != EINVAL)
      fprintf(stderr, "Cannot set signal handler for %d: %s.\n", s, strerror(errno));
}

void dumpStuff(int s, siginfo_t *info)
{
   if (s == SIGSEGV)
      printf("signal handler: Program Terminated due to Segment Violation at address %p.\n", info->si_addr);
   else
      printf("signal handler: Program Terminated due to signal %d.\n", s);
}

void dumpstack()
{
   static char fname[1024];
   int thId = syscall(SYS_gettid);
   sprintf(fname, "stackdump.%d.%d.log", getpid(), thId);

   FILE *fp = fopen(fname, "a");
   if (fp == NULL)
   {
      fprintf(stderr, "Fail to create file'%s'...\n", fname);
      return;
   }

   printf("Saving stack at fault to file '%s'...\n", fname);
   fprintf(fp, "--------------------\n");

   void *bt[1024];
   int size = backtrace(bt, 64 /*maximum number of addresses*/);
   backtrace_symbols_fd(bt, size, fileno(fp));

   fclose(fp);
   fflush(stderr);
   fflush(stdout);
}

void myAbort()
{
   static bool aborting = false;
   if (aborting)
   {
      sleep(1);
      return;
   }
   aborting = true;
   signal(SIGABRT, SIG_DFL);
   std::abort();
}

void dumpStackThenAbort(int sig, siginfo_t *info, void *)
{
   dumpStuff(sig, info);
   dumpstack();
   myAbort();
}

void init()
{
   CreateSignalHandler(SIGABRT, dumpStackThenAbort);
   CreateSignalHandler(SIGBUS, dumpStackThenAbort);
   CreateSignalHandler(SIGILL, dumpStackThenAbort);
   CreateSignalHandler(SIGSEGV, dumpStackThenAbort);
   CreateSignalHandler(SIGFPE, dumpStackThenAbort);
}

void run()
{
   while(1)
   {
      printf("sleep\n");
      sleep(5);
   }

   int a = 1;
   int b = 0;
   printf("%d", a / b);

   int *i = nullptr;
   printf("%d\n", *i);
}

int main()
{
   init();

   run();
   return 0;
}
