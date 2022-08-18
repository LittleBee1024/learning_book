#include "./sim_interface.h"

namespace SIM
{

   bool SimInterface::checkCond(cc_t cc, COND cType)
   {
      bool zf = GET_ZF(cc);
      bool sf = GET_SF(cc);
      bool of = GET_OF(cc);
      bool jump = false;

      switch (cType)
      {
      case C_YES:
         jump = true;
         break;
      case C_LE:
         jump = (sf ^ of) | zf;
         break;
      case C_L:
         jump = sf ^ of;
         break;
      case C_E:
         jump = zf;
         break;
      case C_NE:
         jump = zf ^ 1;
         break;
      case C_GE:
         jump = sf ^ of ^ 1;
         break;
      case C_G:
         jump = (sf ^ of ^ 1) & (zf ^ 1);
         break;
      default:
         jump = false;
         break;
      }
      return jump;
   }

   word_t SimInterface::computeALU(ALU op, word_t argA, word_t argB)
   {
      word_t val;
      switch (op)
      {
      case A_ADD:
         val = argA + argB;
         break;
      case A_SUB:
         val = argB - argA;
         break;
      case A_AND:
         val = argA & argB;
         break;
      case A_XOR:
         val = argA ^ argB;
         break;
      default:
         val = 0;
      }
      return val;
   }

   cc_t SimInterface::computeCC(ALU op, word_t argA, word_t argB)
   {
      word_t val = computeALU(op, argA, argB);
      bool zero = (val == 0);
      bool sign = (val < 0);
      bool ovf;
      switch (op)
      {
      case A_ADD:
         ovf = ((argA < 0) == (argB < 0)) &&
               ((val < 0) != (argA < 0));
         break;
      case A_SUB:
         ovf = ((argA > 0) == (argB < 0)) &&
               ((val < 0) != (argB < 0));
         break;
      case A_AND:
      case A_XOR:
         ovf = false;
         break;
      default:
         ovf = false;
      }
      return PACK_CC(zero, sign, ovf);
   }

};
