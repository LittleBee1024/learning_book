char* str = "Hello World\n";

void print()
{
   // int write(int fd, char* buffer, int size);
   // write(rdi, rsi, rdx);
   //    fd = stdout (0)
   //    buffer = str
   //    size = 13
   // WRITE system call ID = 1 (rax)
   asm(
      "movq $13,%%rdx \n\t"
      "movq %0,%%rsi \n\t"
      "movq $0,%%rdi \n\t"
      "movq $1,%%rax \n\t"
      "syscall \n\t"
      ::"r"(str):"rdi","rsi","rdx"
   );
}

void exit()
{
   // Syscall 60 is exit on x86_64
   // __asm ("movq $60,%rax; movq $42,%rdi; syscall");
   asm(
      "movq $42,%rdi \n\t"
      "movq $60,%rax \n\t"
      "syscall \n\t"
   );
}

void nomain()
{
   print();
   exit();
}
