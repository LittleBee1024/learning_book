#pragma once

#include "./sim_interface.h"
#include "./state.h"
#include "./storage.h"

#include "io_interface.h"
#include "isa.h"

namespace SIM
{
   // Yis simulates instruction one by one, doesn't create an universal model for all instructions
   class Yis : public SimInterface
   {
   public:
      explicit Yis(IO::OutputInterface &out);

      int loadCode(const char *fname) override;
      State runOneStep() override;
      void compare(const SimInterface &other) const override;

   private:
      void compareReg(const Yis &other) const;
      void compareMem(const Yis &other) const;

   private:
      IO::OutputInterface &m_out;

      word_t m_pc;
      RegStore m_reg;
      MemStore m_mem;
      cc_t m_cc;
   };

}
