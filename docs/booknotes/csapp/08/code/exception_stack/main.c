#include <stdio.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <errno.h>
#include <execinfo.h>
#include <string.h>

typedef void (*SigactionHandler)(int, siginfo_t *, void *);
void createSignalHandler(int s, SigactionHandler handler)
{
   struct sigaction sa
   {
   };
   // SA_RESTART - restart syscalls if possible
   sa.sa_flags = SA_SIGINFO | SA_RESTART;
   // sa_mask: the signal which triggered the handler will be blocked, unless the SA_NODEFER flag is used
   sigemptyset(&sa.sa_mask); // Block sigs of type being handled
   sa.sa_sigaction = handler;
   int rc = sigaction(s, &sa, NULL);
   if (rc != 0 && errno != EINVAL)
      fprintf(stderr, "Cannot set signal handler for %d: %s.\n", s, strerror(errno));
}

void dumpStuff(int s, siginfo_t *info)
{
   if (s == SIGSEGV)
      fprintf(stderr, "signal handler: Program Terminated due to Segment Violation at address %p.\n", info->si_addr);
   else
      fprintf(stderr, "signal handler: Program Terminated due to signal %d.\n", s);
}

void dumpStack()
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

   fprintf(stderr, "Saving stack at fault to file '%s'...\n", fname);

   void *bt[1024];
   int size = backtrace(bt, 64 /*maximum number of addresses*/);
   backtrace_symbols_fd(bt, size, fileno(fp));

   fprintf(fp, "--------------------\n");
   fclose(fp);

   fflush(stderr);
}

void abort()
{
   // reset SIGABRT signal handler to default one in case that it is triggered recursively
   signal(SIGABRT, SIG_DFL);
   std::abort();
}

void dumpStackThenAbort(int sig, siginfo_t *info, void *)
{
   dumpStuff(sig, info);
   dumpStack();
   abort();
}

void init()
{
   createSignalHandler(SIGABRT, dumpStackThenAbort);
   createSignalHandler(SIGBUS, dumpStackThenAbort);
   createSignalHandler(SIGILL, dumpStackThenAbort);
   createSignalHandler(SIGSEGV, dumpStackThenAbort);
   createSignalHandler(SIGFPE, dumpStackThenAbort);
}

void run()
{
   /* Test1: SIGSEGV signal */
   int *i = nullptr;
   printf("%d\n", *i);

   /* Test2: SIGFPE signal */
   int a = 1;
   int b = 0;
   printf("%d", a / b);

   /* Test3: any signal using "kill -<signum> <pid>" */
   while(1)
   {
      printf("sleep\n");
      sleep(5);
   }
}

int main()
{
   init();
   run();
   return 0;
}
