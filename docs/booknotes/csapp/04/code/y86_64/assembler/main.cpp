#include "./input.h"
#include "./output.h"
#include "./yas.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>

static void usage(char *pname)
{
   printf("Usage: %s file.ys\n", pname);
   exit(0);
}

static bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() &&
          0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

int main(int argc, char *argv[])
{
   if (argc == 1)
   {
      usage(argv[0]);
      return 0;
   }

   std::string infname = argv[1];
   if (!endsWith(infname, ".ys"))
      usage(argv[0]);

   std::string outfname = infname.substr(0, infname.find_last_of('.')) + ".yo";

   YAS::Lexer lex(std::move(std::make_unique<YAS::FileIn>(infname.c_str())));
   int ret = lex.parse(std::move(std::make_unique<YAS::FileOut>(outfname.c_str())));

   if (ret == YAS::ERROR)
      printf("Yas Lexer parse has error\n");
   else
      printf("Yas Lexer parse is done\n");

   return 0;
}
