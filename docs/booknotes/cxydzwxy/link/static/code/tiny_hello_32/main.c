char* str = "Hello World\n";

void print()
{
   // int write(int fd, char* buffer, int size);
   // write(ebx, ecx, edx);
   //    fd = stdout (0)
   //    buffer = str
   //    size = 13
   // WRITE system call ID = 4 (eax)
   asm(
      "movl $13,%%edx \n\t"
      "movl %0,%%ecx \n\t"
      "movl $0,%%ebx \n\t"
      "movl $4,%%eax \n\t"
      "int $0x80 \n\t"
      ::"r"(str):"edx","ecx","ebx"
   );
}

void exit()
{
   // exit()
   // EXIT system call ID = 1 (eax)
   // return value 42
   asm(
      "movl $42,%ebx \n\t"
      "movl $1,%eax \n\t"
      "int $0x80 \n\t"
   );
}

void nomain()
{
   print();
   exit();
}
