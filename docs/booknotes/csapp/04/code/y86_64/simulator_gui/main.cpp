#include "./simulator_gui/c_api_sim.h"

#include <assert.h>

int main(int argc, char *argv[])
{
   int rc = 0;

   rc = sim_yis_reset();
   assert(rc == 0);

   rc = sim_yis_load_code("./code/asum.yo");
   assert(rc == 0);

   rc = sim_yis_run();
   assert(rc == 0);

   return 0;
}
