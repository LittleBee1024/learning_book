#include <stdio.h>
#include <setjmp.h> // jmp_buf

#define ERR_FOO 1
#define ERR_BAR 2

int error_foo_trigger = 0;
int error_bar_trigger = 1;

jmp_buf buf;

void bar(void)
{
   if (error_bar_trigger)
      longjmp(buf, ERR_BAR);
}

void foo(void)
{
   if (error_foo_trigger)
      longjmp(buf, ERR_FOO);
   bar();
}

int main()
{
   switch (setjmp(buf))
   {
   case 0:
      foo();
      break;
   case ERR_FOO:
      printf("Detected an error_foo condition in foo\n");
      break;
   case ERR_BAR:
      printf("Detected an error_bar condition in foo\n");
      break;
   default:
      printf("Unknown error condition in foo\n");
   }

   return 0;
}
