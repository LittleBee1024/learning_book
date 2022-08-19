#include "./sim_seq.h"

namespace SIM
{

   Seq::Seq(IO::OutputInterface &out) : m_out(out)
   {
   }

   int Seq::loadCode(const char *fname)
   {
      return 0;
   }

   State Seq::runOneStep()
   {
      return STAT_OK;
   }

   void Seq::compare(const SimInterface &other) const
   {
   }
}
