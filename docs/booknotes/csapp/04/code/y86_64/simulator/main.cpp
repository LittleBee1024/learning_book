#include "./simulator/sim_interface.h"
#include "./simulator/sim_yis.h"
#include "./simulator/sim_seq.h"
#include "./simulator/sim_pipe.h"
#include "./simulator/state.h"
#include "./_common/io_interface.h"
#include "./_common/output.h"

#include <memory>
#include <unistd.h>
#include <assert.h>

void usage(char *pname)
{
   printf("Usage: %s code_file [-y/s/p] [-m max_cycles]\n", pname);
   printf("\tcode_file  : Y86-64 code file *.yo\n");
   printf("\t-y         : YIS simulator\n");
   printf("\t-s         : SEQ simulator\n");
   printf("\t-p         : PIPE simulator\n");
   printf("\t-m         : specifies the max number of running steps\n");
}

bool endsWith(const std::string &str, const std::string &suffix)
{
   return str.size() >= suffix.size() && 0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

enum class SIMType
{
   YIS,
   SEQ,
   PIPE
};

std::unique_ptr<SIM::SimInterface> createSimulator(SIMType type, std::shared_ptr<IO::OutputInterface> out)
{
   switch (type)
   {
   case SIMType::YIS:
      out->out("[INFO] Create Yis Simulator\n");
      return std::make_unique<SIM::Yis>(out);
   case SIMType::SEQ:
      out->out("[INFO] Create SEQ Simulator\n");
      return std::make_unique<SIM::Seq>(out);
   case SIMType::PIPE:
      out->out("[INFO] Create Pipeline Simulator\n");
      return std::make_unique<SIM::Pipe>(out);
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
   case SIMType::PIPE:
   {
      auto child = dynamic_cast<const SIM::Pipe *>(sim);
      assert(child != nullptr);
      return std::make_unique<SIM::Pipe>(*child);
   }
   default:
      assert(0 && "Invalid Simulator Type\n");
      return nullptr;
   }
}

struct Options
{
   std::string infname;
   int maxCycles = 10000;
   SIMType type = SIMType::YIS;
};

int main(int argc, char *argv[])
{
   Options option;
   int ch;
   while ((ch = getopt(argc, argv, "hyspm:")) != -1)
   {
      switch (ch)
      {
      case 'h':
         usage(argv[0]);
         return 0;
      case 'm':
         option.maxCycles = std::stoi(optarg);
         break;
      case 'y':
         option.type = SIMType::YIS;
         break;
      case 's':
         option.type = SIMType::SEQ;
         break;
      case 'p':
         option.type = SIMType::PIPE;
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

   std::shared_ptr<IO::OutputInterface> out = std::make_shared<IO::StdOut>();
   std::unique_ptr<SIM::SimInterface> sim = createSimulator(option.type, out);

   if (!sim->loadCode(option.infname.c_str()))
   {
      out->out("[ERROR] Failed to load code\n");
      return -1;
   }

   std::unique_ptr<SIM::SimInterface> snapshot = takeSnapshot(option.type, sim.get());

   sim->run(option.maxCycles);
   sim->compare(*snapshot);

   return 0;
}
