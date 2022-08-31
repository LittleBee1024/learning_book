#include "./simulator_gui/singleton.h"
#include "./_common/output.h"

#include <stdarg.h>

#define SYS_LOG "./sys.log"

namespace SIM
{
   std::shared_ptr<IO::OutputInterface> simOut = std::make_shared<IO::FileOut>(SYS_LOG);

   Yis *YisSingleton::getInstance()
   {
      static Yis yis(simOut);
      return &yis;
   }

   // void YisSingleton::out(const char *format, ...)
   // {
   //    static char buffer[1024];
   //    va_list args;
   //    va_start(args, format);
   //    s_out->out("%s", buffer);
   //    va_end(args);
   // }

}
