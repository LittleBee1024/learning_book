#include <stdio.h>
#include <setjmp.h> // jmp_buf

jmp_buf buf;

#define ERROR 1

int error = 1;

void foo(void)
{
   if (error)
      longjmp(buf, ERROR);
}

void bar(void)
{
   foo();
   printf("End bar\n");
}

int main()
{
   switch (setjmp(buf))
   {
   case 0:
      bar();
      break;
   case ERROR:
      printf("Detected an error condition in foo\n");
      break;
   default:
      printf("Unknown error condition in foo\n");
   }

   return 0;
}
