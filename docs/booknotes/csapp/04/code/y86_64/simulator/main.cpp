#include "io_interface.h"
#include "output.h"
#include "./sim_interface.h"
#include "./sim_yis.h"
#include "./sim_seq.h"
#include "./state.h"

#include <memory>
#include <unistd.h>
#include <assert.h>

void usage(char *pname)
{
   printf("Usage: %s code_file [-m max_steps]\n", pname);
}

bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

enum class SIMType
{
   YIS,
   SEQ
};

std::unique_ptr<SIM::SimInterface> createSimulator(SIMType type, IO::OutputInterface &out)
{
   switch (type)
   {
   case SIMType::YIS:
      return std::make_unique<SIM::Yis>(out);
   case SIMType::SEQ:
      return std::make_unique<SIM::Seq>(out);
   default:
      assert(0 && "Invalid Simulator Type\n");
      return nullptr;
   }
}

std::unique_ptr<SIM::SimInterface> takeSnapshot(SIMType type, const SIM::SimInterface *sim)
{
   switch (type)
   {
   case SIMType::YIS:
   {
      auto child = dynamic_cast<const SIM::Yis *>(sim);
      assert(child != nullptr);
      return std::make_unique<SIM::Yis>(*child);
   }
   case SIMType::SEQ:
   {
      auto child = dynamic_cast<const SIM::Seq *>(sim);
      assert(child != nullptr);
      return std::make_unique<SIM::Seq>(*child);
   }
   default:
      assert(0 && "Invalid Simulator Type\n");
      return nullptr;
   }
}

struct Options
{
   std::string infname;
   int maxSteps = 10000;
   SIMType type = SIMType::YIS;
};

int main(int argc, char *argv[])
{
   Options option;
   int ch;
   while ((ch = getopt(argc, argv, "hysm:")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      case 'm':
         option.maxSteps = std::stoi(optarg);
         break;
      case 'y':
         option.type = SIMType::YIS;
         break;
      case 's':
         option.type = SIMType::SEQ;
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
   std::unique_ptr<SIM::SimInterface> sim = createSimulator(option.type, *out);

   if (!sim->loadCode(option.infname.c_str()))
   {
      out->out("[ERROR] Failed to load code\n");
      return -1;
   }

   std::unique_ptr<SIM::SimInterface> snapshot = takeSnapshot(option.type, sim.get());

   sim->run(option.maxSteps);
   sim->compare(*snapshot);

   return 0;
}
