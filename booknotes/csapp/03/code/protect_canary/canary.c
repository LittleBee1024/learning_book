void foo(long *x)
{
   *x += 1;
}

void bar()
{
   long x = 1;
   foo(&x);
}
