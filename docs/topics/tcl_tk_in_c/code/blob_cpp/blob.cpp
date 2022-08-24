#include "./blob.h"

#include <stdio.h>

Blob::Blob()
{
   printf("[ INFO] Construct Blob\n");
}

Blob::~Blob()
{
   printf("[ INFO] Destruct Blob\n");
}

int Blob::add(const char *name, int age)
{
   if (age <= 0)
   {
      printf("[ERROR] The age of %s should be greater than zero\n", name);
      return -1;
   }

   printf("[ INFO] Add %s to the Blob, his age is %d\n", name, age);
   m_ages[name] = age;
   return 0;
}

int Blob::find(const char *name)
{
   auto iter = m_ages.find(name);
   if (iter == m_ages.end())
   {
      printf("[ WARN] Fail to find %s in the Blob\n", name);
      return -1;
   }
   int age = m_ages[name];
   printf("[ INFO] Find %s in the Blob, his age is %d\n", name, age);
   return age;
}

void Blob::remove(const char *name)
{
   auto iter = m_ages.find(name);
   if (iter == m_ages.end())
   {
      printf("[ WARN] Cannot remove %s from the Blob because it doesn't exist\n", name);
      return;
   }
   printf("[ INFO] Remove %s from the Blob, his age is %d\n", name, m_ages[name]);
   m_ages.erase(name);
}
