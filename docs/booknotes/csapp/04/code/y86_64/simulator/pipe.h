#pragma once

#include "./pipe_regs.h"
#include "isa.h"

/* Defined in sim_pipe.cpp */
namespace PIPE
{
   /* Defined in pipe.cpp which is generated from pipe.hcl by hcl tool */
   word_t gen_f_pc();
   word_t gen_f_predPC();
   word_t gen_f_icode();
   word_t gen_f_ifun();
   word_t gen_f_stat();
   word_t gen_need_regids();
   word_t gen_need_valC();
   word_t gen_instr_valid();

   word_t gen_d_srcA();
   word_t gen_d_srcB();
   word_t gen_d_dstE();
   word_t gen_d_dstM();
   word_t gen_d_valA();
   word_t gen_d_valB();
   word_t gen_w_dstE();
   word_t gen_w_valE();
   word_t gen_w_dstM();
   word_t gen_w_valM();
   word_t gen_Stat();

   word_t gen_F_stall();
   word_t gen_F_bubble();
   word_t gen_D_stall();
   word_t gen_D_bubble();
   word_t gen_E_stall();
   word_t gen_E_bubble();
   word_t gen_M_stall();
   word_t gen_M_bubble();
   word_t gen_W_stall();
   word_t gen_W_bubble();

   /* Intermdiate stage values that must be used by control functions */
   extern byte_t imem_icode;
   extern byte_t imem_ifun;
   extern bool imem_error;
   extern bool instr_valid;
   extern word_t d_regvala;
   extern word_t d_regvalb;
   extern bool dmem_error;

   // export pipeline registers as a global varible
   extern SIM::PipeRegs pipe_regs;
}
