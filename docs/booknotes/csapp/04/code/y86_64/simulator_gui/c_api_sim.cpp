#include "./simulator_gui/c_api_sim.h"
#include "./simulator_gui/singleton.h"
#include "./simulator/state.h"

#define C_OK 0
#define C_ERR -1

int sim_reset()
{
   auto yis = SIM::SimSingleton::getInstance();
   yis->reset();
   yis->recover();
   return C_OK;
}

int sim_load_code(const char *filename)
{
   auto yis = SIM::SimSingleton::getInstance();
   if (yis->loadCode(filename) == 0)
   {
      G_SIM_LOG("[ERROR] Failed to load code file %s\n", filename);
      return C_ERR;
   }
   yis->save();
   return C_OK;
}

const char *sim_step_run(int step_num)
{
   auto yis = SIM::SimSingleton::getInstance();

   SIM::State state = SIM::STAT_OK;
   int i = 0;
   for (i = 0; i < step_num && state == SIM::STAT_OK; i++)
   {
      state = yis->runOneCycle();
      G_SIM_LOG("[INFO] The cycle is done with State=%s\n", SIM::getStateName(state));
   }

   if (i != step_num && state != SIM::STAT_HLT)
   {
      G_SIM_LOG("[ERROR] Step %d fails\n", i);
      return SIM::getStateName(state);
   }

   return SIM::getStateName(state);
}

int sim_diff()
{
   auto yis = SIM::SimSingleton::getInstance();
   yis->diffReg();
   yis->diffMem();
   return C_OK;
}
