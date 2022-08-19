#include "io_interface.h"
#include "output.h"
#include "./sim_interface.h"
#include "./sim_yis.h"
#include "./state.h"

#include <memory>
#include <unistd.h>

void usage(char *pname)
{
   printf("Usage: %s code_file [-m max_steps]\n", pname);
}

static bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

struct Options
{
   std::string infname;
   int maxSteps = 10000;
};

int main(int argc, char *argv[])
{
   Options option;
   int ch;
   while ((ch = getopt(argc, argv, "hm:")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      case 'm':
         option.maxSteps = std::stoi(optarg);
         break;
      }
   }

   option.infname = argv[optind];
   if (optind != argc - 1 || !endsWith(option.infname, ".yo"))
   {
      usage(argv[0]);
      return -1;
   }
   if (access(option.infname.c_str(), F_OK) != 0)
   {
      fprintf(stderr, "Cannot access '%s': No such file\n", option.infname.c_str());
      return -1;
   }

   std::unique_ptr<IO::OutputInterface> out = std::make_unique<IO::StdOut>();
   SIM::Yis sim = SIM::Yis(*out);

   if (!sim.loadCode(option.infname.c_str()))
   {
      out->out("[ERROR] Failed to load code\n");
      return -1;
   }

   SIM::Yis snapshot(sim);

   SIM::State state = SIM::STAT_OK;
   int i = 0;
   for (i = 0; i < option.maxSteps && state == SIM::STAT_OK; i++)
      state = sim.runOneStep();

   out->out("After %d steps, the state becomes %s\n", i, SIM::getStateName(state));

   sim.compare(snapshot);

   return 0;
}
