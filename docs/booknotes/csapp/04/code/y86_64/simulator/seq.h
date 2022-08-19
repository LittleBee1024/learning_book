#pragma once

#include "isa.h"

namespace SEQ
{

   /* SEQ+ functions from HCL */
   word_t gen_pc();
   word_t gen_icode();
   word_t gen_ifun();
   word_t gen_need_regids();
   word_t gen_need_valC();
   word_t gen_instr_valid();
   word_t gen_srcA();
   word_t gen_srcB();
   word_t gen_dstE();
   word_t gen_dstM();
   word_t gen_aluA();
   word_t gen_aluB();
   word_t gen_alufun();
   word_t gen_set_cc();
   word_t gen_mem_addr();
   word_t gen_mem_data();
   word_t gen_mem_read();
   word_t gen_mem_write();
   word_t gen_Stat();

   /* SEQ+ variables from HCL */

   /* Results computed by previous instruction. Used to compute PC in current instruction */
   extern byte_t prev_icode;
   extern byte_t prev_ifun;
   extern word_t prev_valc;
   extern word_t prev_valm;
   extern word_t prev_valp;
   extern bool prev_bcond;

   /* Intermdiate stage values that must be used by control functions */
   extern byte_t imem_icode;
   extern byte_t imem_ifun;
   extern byte_t icode;
   extern word_t ifun;
   extern word_t ra;
   extern word_t rb;
   extern word_t valc;
   extern word_t valp;
   extern bool imem_error;
   extern bool instr_valid;
   extern word_t vala;
   extern word_t valb;
   extern word_t vale;
   extern bool bcond;
   extern bool cond;
   extern word_t valm;
   extern bool dmem_error;
   extern byte_t status;

}
