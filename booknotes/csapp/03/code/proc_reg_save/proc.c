long Q(long y)
{
   return y + 1;
}

long P(long x, long y)
{
   long u = Q(y);
   long v = Q(x);
   return u + v;
}

