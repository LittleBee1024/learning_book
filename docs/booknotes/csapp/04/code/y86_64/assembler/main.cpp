#include "./input.h"
#include "./output.h"
#include "./yas.h"

#include <fstream>
#include <iostream>
#include <stdio.h>
#include <string>
#include <memory>
#include <unistd.h>

static void usage(char *pname)
{
   printf("Usage: %s <filename>.ys [-o <filename>.yo] [-h]\n", pname);
}

static bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() &&
          0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

struct Options
{
   std::string infname;
   std::string outfname;
};

int main(int argc, char *argv[])
{
   Options option;
   int ch;
   while ((ch = getopt(argc, argv, "o:h")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      case 'o':
         option.outfname = optarg;
         break;
      }
   }

   option.infname = argv[optind];
   if (optind != argc - 1 || !endsWith(option.infname, ".ys"))
   {
      usage(argv[0]);
      return -1;
   }
   if (access(option.infname.c_str(), F_OK) != 0)
   {
      fprintf(stderr, "Cannot access '%s': No such file\n", option.infname.c_str());
      return -1;
   }

   YAS::Lexer lex(std::move(std::make_unique<YAS::FileIn>(option.infname.c_str())));

   std::unique_ptr<YAS::OutputInterface> out;
   if (option.outfname.empty())
      out = std::make_unique<YAS::StdOut>();
   else
      out = std::make_unique<YAS::FileOut>(option.outfname.c_str());

   int rc = lex.parse(std::move(out));

   if (rc == YAS::ERROR)
      printf("Yas Lexer parse has error\n");
   else
      printf("Yas Lexer parse is done\n");

   return rc;
}
