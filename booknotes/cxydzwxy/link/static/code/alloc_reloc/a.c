extern int swap();
extern int shared;

int main()
{
   shared = 2;
   swap();
   return 0;
}
