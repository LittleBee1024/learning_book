#pragma once

#include "./def.h"

namespace ISA
{

   /**
    * @brief Y86-64 Registers
    */
   struct Register
   {
      const char *name;
      REG_ID id;
   };

   /**
    * @brief Find Y86-64 reigster ID by register name
    * 
    * @param name Register name
    * @return Y_REG_ID, for invalid register name, return Y_REG_ID::ERR
    */
   REG_ID findRegister(const char *name);

   /**
    * @brief Get the Y86-64 register name by register ID
    * 
    * @param id Register ID
    * @return const char*, for invalid ID, return nullptr
    */
   const char *getRegName(REG_ID id);

}
