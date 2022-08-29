#pragma once

#include <string>
#include <unordered_map>

class Blob
{
public:
   Blob();
   ~Blob();

   // return 0 if success, -1 if the age is invalid
   int add(const char *name, int age);
   // return -1 if cannot find the name, otherwise return his age
   int find(const char *name);
   void remove(const char *name);

private:
   std::unordered_map<std::string, int> m_ages;
};
