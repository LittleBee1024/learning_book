void switch_eg_impl(long x, long n, long *dest)
{
   static void *jt[7] = {
      &&loc_A, &&loc_def, &&loc_B, &&loc_C, &&loc_D, &&loc_def, &&loc_D
   };
   unsigned long index = n - 100;
   long val;
   if(index > 6)
      goto loc_def;
   goto *jt[index];

loc_A:
   val = x * 13;
   goto done;
loc_B:
   x = x + 10;
loc_C:
   val = x + 11;
   goto done;
loc_D:
   val = x * x;
   goto done;
loc_def:
   val = 0;
done:
   *dest = val;
}
