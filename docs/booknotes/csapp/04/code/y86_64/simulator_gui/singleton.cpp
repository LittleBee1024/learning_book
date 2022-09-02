#include "./simulator_gui/singleton.h"
#include "./_common/output.h"

#include <stdarg.h>

#define SYS_LOG "./sys.log"

namespace SIM
{
   std::shared_ptr<IO::OutputInterface> g_out = std::make_shared<IO::FileOut>(SYS_LOG);

   Yis *SimSingleton::getInstance()
   {
      static Yis yis(g_out);
      return &yis;
   }
}
