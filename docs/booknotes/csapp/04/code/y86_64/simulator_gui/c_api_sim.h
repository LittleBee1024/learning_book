#pragma once

#include <cstdint>

extern "C"
{
   /**
    * @brief Reset simulator
    * 
    * @return 0: SUCESS, -1: ERROR
    */
   int sim_reset();

   /**
    * @brief Load machine code file to simulator
    * 
    * @param filename The machine code filename(./yo)
    * @return 0: SUCESS, -1: ERROR
    */
   int sim_load_code(const char *filename);

   /**
    * @brief Run simulator in serveral steps after loading code
    * 
    * @param step_num The number of running steps
    * @return 0: SUCESS, -1: ERROR
    */
   int sim_step_run(int step_num);

   /**
    * @brief Dump the differences in registers and memory after running a program
    * 
    * @return 0: SUCESS, -1: ERROR
    */
   int sim_diff();
}
