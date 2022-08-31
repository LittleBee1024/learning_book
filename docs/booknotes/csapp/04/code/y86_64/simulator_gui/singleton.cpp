#include "./simulator_gui/singleton.h"
#include "./simulator/sim_yis.h"
#include "./_common/output.h"

#include <stdarg.h>

#define SYS_LOG "./sys.log"

namespace SIM
{
   std::shared_ptr<IO::OutputInterface> simOut = std::make_shared<IO::FileOut>(SYS_LOG);

   SimInterface *SimSingleton::getInstance()
   {
      static Yis yis(simOut);
      return &yis;
   }
}
