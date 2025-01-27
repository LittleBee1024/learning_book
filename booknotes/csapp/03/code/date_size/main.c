#include <stdio.h>

struct A
{
   char a;     // 0
   short b;    // 2
   int c;      // 4
}; // 8

struct B
{
   char a;     // 0
   int b;      // 4
   short c;    // 8
}; // 12

#pragma pack(2)
struct C
{
   char a;     // 0
   int b;      // 2
   short c;    // 6
}; // 8
#pragma pack()

struct D
{
   double a;   // 0
   char b;     // 8
}; // 16

#pragma pack(4)
struct E
{
   double a;   // 0
   char b;     // 8
}; // 12
#pragma pack()

int main()
{
   printf("size class A: %zu\n", sizeof(struct A));
   printf("size class B: %zu\n", sizeof(struct B));
   printf("size class C: %zu\n", sizeof(struct C));
   printf("size class D: %zu\n", sizeof(struct D));
   printf("size class E: %zu\n", sizeof(struct E));
}
