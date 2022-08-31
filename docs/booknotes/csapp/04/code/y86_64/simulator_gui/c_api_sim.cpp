#include "./simulator_gui/c_api_sim.h"
#include "./simulator_gui/singleton.h"
#include "./simulator/state.h"

#define C_OK 0
#define C_ERR -1

int sim_reset()
{
   auto yis = SIM::SimSingleton::getInstance();
   yis->reset();
   return C_OK;
}

int sim_load_code(const char *filename)
{
   auto yis = SIM::SimSingleton::getInstance();
   if (yis->loadCode(filename) == 0)
   {
      G_SIM_LOG("[ ERR] Failed to load code file %s\n", filename);
      return C_ERR;
   }
   return C_OK;
}

int sim_step_run(int step_num)
{
   auto yis = SIM::SimSingleton::getInstance();

   SIM::State state = SIM::STAT_OK;
   int i = 0;
   for (i = 0; i < step_num && state == SIM::STAT_OK; i++)
   {
      state = yis->runOneCycle();
      G_SIM_LOG("[INFO] The cycle is done with State=%s\n", SIM::getStateName(state));
   }

   if (state == SIM::STAT_HLT)
      return C_OK;

   if (i != step_num)
   {
      G_SIM_LOG("[ ERR] Step %d fails\n", i);
      return C_ERR;
   }

   return C_OK;
}
