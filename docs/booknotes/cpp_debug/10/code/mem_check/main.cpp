#include <stdio.h>

int main()
{
   int *array = new int[100];
   array[100] = 0; // error
   delete[] array;
   return 0;
}
