#pragma once

#include "./sim_interface.h"
#include "./seq.h"
#include "./state.h"

#include "io_interface.h"

namespace SIM
{
   // Seq simulator is the implementation of SEQ+ model in the book
   class Seq : public SimInterface
   {
   public:
      explicit Seq(IO::OutputInterface &out);

      int loadCode(const char *fname) override;
      State runOneStep() override;
      void compare(const SimInterface &other) const override;

   private:
      IO::OutputInterface &m_out;
   };

}
