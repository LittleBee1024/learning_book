#include <unistd.h>
#include <stdio.h>
#include <sys/syscall.h>
#include <stdint.h>

// https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html
// Common constraints include ‘r’ for register and ‘m’ for memory. When you list more than one possible
// location (for example, "=rm"), the compiler chooses the most efficient one based on the current context.
int asm_write(int fd, const void* buffer, size_t size)
{
   int ret = 0;

   // write(rdi, rsi, rdx);
   // WRITE system call ID = 1 (rax)
   asm("movl %1,%%edi \n\t"
       "movq %2,%%rsi \n\t"
       "movq %3,%%rdx \n\t"
       "movq $1,%%rax \n\t"
       "syscall       \n\t"
       "movl %%eax,%0 \n\t"
       :"=rm"(ret)
       :"rm"(fd),"rm"(buffer),"rm"(size));

   return ret;
}

int main()
{
   int rc = 0;

   rc = printf("hello, world0\n");
   printf("\treturn code = %d\n", rc);

   rc = write(1/*fileno(stdout)*/, "hello, world1\n", 14);
   printf("\treturn code = %d\n", rc);

   rc = syscall(SYS_write, 1, "hello, world2\n", 14);
   printf("\treturn code = %d\n", rc);

   rc = asm_write(1, "hello, world3\n", 14);
   printf("\treturn code = %d\n", rc);

   _exit(0);
}
