#include "io_interface.h"
#include "input.h"
#include "output.h"
#include "./hcl.h"

#include <string>
#include <memory>
#include <unistd.h>

static void usage(char *pname)
{
   printf("Usage: %s <filename>.hcl [-c/v] [-o <filename>]\n", pname);
   printf("\tExample: generate C code from HCL code\n");
   printf("\t\t> hcl example.hcl -c -o example.c\n");
   printf("\tExample: generate Verilog code from HCL code\n");
   printf("\t\t> hcl example.hcl -v -o example.v\n");
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
   bool toVerilog = false;
};

int main(int argc, char *argv[])
{
   Options option;
   int ch;
   while ((ch = getopt(argc, argv, "o:hcv")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      case 'c':
         option.toVerilog = false;
         break;
      case 'v':
         option.toVerilog = true;
         break;
      case 'o':
         option.outfname = optarg;
         break;
      }
   }

   option.infname = argv[optind];
   if (optind != argc - 1 || !endsWith(option.infname, ".hcl"))
   {
      usage(argv[0]);
      return -1;
   }
   if (access(option.infname.c_str(), F_OK) != 0)
   {
      fprintf(stderr, "Cannot access '%s': No such file\n", option.infname.c_str());
      return -1;
   }

   HCL::Parser parser(std::move(std::make_unique<IO::FileIn>(option.infname.c_str())));

   std::unique_ptr<IO::OutputInterface> out;
   if (option.outfname.empty())
      out = std::make_unique<IO::StdOut>();
   else
      out = std::make_unique<IO::FileOut>(option.outfname.c_str());

   int rc = 0;
   if (!option.toVerilog)
      rc = parser.toC(std::move(out));
   else
      rc = parser.toVerilog(std::move(out));

   if (rc == HCL::ERROR)
      printf("HCL parse has error\n");
   else
      printf("HCL parse is done\n");

   return 0;
}
