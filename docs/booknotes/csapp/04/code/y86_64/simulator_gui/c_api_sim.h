#pragma once

#include <cstdint>

extern "C"
{
   /**
    * @brief Reset YIS simulator
    * 
    * @return 0: SUCESS, -1: ERROR
    */
   int sim_yis_reset();

   /**
    * @brief Load machine code file to YIS simulator
    * 
    * @param filename The machine code filename(./yo)
    * @return 0: SUCESS, -1: ERROR
    */
   int sim_yis_load_code(const char *filename);

   /**
    * @brief Run YIS simulator in serveral steps after loading code
    * 
    * @param step_num The number of running steps
    * @return 0: SUCESS, -1: ERROR
    */
   int sim_yis_step_run(int step_num);
}

