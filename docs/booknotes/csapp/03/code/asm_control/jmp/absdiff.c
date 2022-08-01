long lt_cnt = 0;
long ge_cnt = 0;

long absdiff(long x, long y)
{
   long result;
   if (x < y)
   {
      lt_cnt++;
      result = y - x;
   }
   else
   {
      ge_cnt++;
      result = x - y;
   }
   return result;
}

long gotodiff(long x, long y)
{
   long result;
   if (x >= y)
      goto x_ge_y;
   lt_cnt++;
   result = y - x;
   return result;
x_ge_y:
   ge_cnt++;
   result = x - y;
   return result;
}
