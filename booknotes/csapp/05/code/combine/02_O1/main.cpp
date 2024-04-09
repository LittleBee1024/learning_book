#include "./misc.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

typedef long data_t;
typedef struct
{
   long len;
   data_t *data;
} vec_rec, *vec_ptr;

vec_ptr new_vec(long len)
{
   assert(len > 0);

   vec_ptr result = (vec_ptr)malloc(sizeof(vec_rec));
   assert(result != nullptr);

   data_t *data = nullptr;
   result->len = len;
   result->data = (data_t *)calloc(len, sizeof(data_t));
   return result;
}

void free_vec(vec_ptr v)
{
   free(v->data);
   free(v);
}

bool get_vec_element(vec_ptr v, long index, data_t *dest)
{
   if (index < 0 || index >= v->len)
      return false;
   *dest = v->data[index];
   return true;
}

bool set_vec_element(vec_ptr v, long index, data_t src)
{
   if (index < 0 || index >= v->len)
      return false;
   v->data[index] = src;
   return true;
}

data_t *get_vec_start(vec_ptr v)
{
   return v->data;
}

long vec_length(vec_ptr v)
{
   return v->len;
}

void combine1(vec_ptr v, data_t *dest)
{
   for (long i = 0; i < vec_length(v); i++)
   {
      data_t val;
      get_vec_element(v, i, &val);
      *dest = (*dest) + val;
   }
}

void combine2(vec_ptr v, data_t *dest)
{
   long len = vec_length(v);
   for (long i = 0; i < len; i++)
   {
      data_t val;
      get_vec_element(v, i, &val);
      *dest = (*dest) + val;
   }
}

void combine3(vec_ptr v, data_t *dest)
{
   long len = vec_length(v);
   data_t *data = get_vec_start(v);
   for (long i = 0; i < len; i++)
   {
      *dest = *dest + data[i];
   }
}

void combine4(vec_ptr v, data_t *dest)
{
   long len = vec_length(v);
   data_t *data = get_vec_start(v);
   data_t acc = *dest;
   for (long i = 0; i < len; i++)
   {
      acc = acc + data[i];
   }
   *dest = acc;
}

void combine5(vec_ptr v, data_t *dest)
{
   long len = vec_length(v);
   data_t *data = get_vec_start(v);
   data_t acc = *dest;

   long i = 0;
   long len_floor = len - (len % 2);
   // combine 2 elements at a time
   for (; i < len; i += 2)
   {
      acc = (acc + data[i]) + data[i+1];
   }
   // finish any remaining elements
   for (; i < len; i++)
   {
      acc += data[i];
   }
   *dest = acc;
}

void combine6(vec_ptr v, data_t *dest)
{
   long len = vec_length(v);
   data_t *data = get_vec_start(v);
   data_t acc0 = *dest;
   data_t acc1 = 0;

   long i = 0;
   long len_floor = len - (len % 2);
   // combine 2 elements at a time
   for (; i < len_floor; i += 2)
   {
      acc0 += data[i];
      acc1 += data[i+1];
   }
   // finish any remaining elements
   for (; i < len; i++)
   {
      acc0 += data[i];
   }

   *dest = acc0 + acc1;
}

void combine7(vec_ptr v, data_t *dest)
{
   long len = vec_length(v);
   data_t *data = get_vec_start(v);
   data_t acc = *dest;

   long i = 0;
   long len_floor = len - (len % 2);
   // combine 2 elements at a time
   for (; i < len; i += 2)
   {
      acc += (data[i] + data[i+1]);
   }
   // finish any remaining elements
   for (; i < len; i++)
   {
      acc += data[i];
   }
   *dest = acc;
}

int main()
{
   long arr_len = 10000001;
   vec_ptr arr = new_vec(arr_len);
   for(long i = 0; i < arr_len; i++)
   {
      data_t v = i + 2;
      bool rc = set_vec_element(arr, i, v);
      assert(rc == true);
   }

   data_t sum1 = 0;
   __TIMER_LOG("combine1 time: ")
   {
      combine1(arr, &sum1);
   }

   data_t sum2 = 0;
   __TIMER_LOG("combine2 time: ")
   {
      combine2(arr, &sum2);
   }

   data_t sum3 = 0;
   __TIMER_LOG("combine3 time: ")
   {
      combine3(arr, &sum3);
   }

   data_t sum4 = 0;
   __TIMER_LOG("combine4 time: ")
   {
      combine4(arr, &sum4);
   }

   data_t sum5 = 0;
   __TIMER_LOG("combine5 time: ")
   {
      combine5(arr, &sum5);
   }

   data_t sum6 = 0;
   __TIMER_LOG("combine6 time: ")
   {
      combine6(arr, &sum6);
   }

   data_t sum7 = 0;
   __TIMER_LOG("combine7 time: ")
   {
      combine7(arr, &sum7);
   }

   free_vec(arr);
   return 0;
}
