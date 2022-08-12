#pragma once

#include "io_interface.h"

#include <memory>

namespace HCL
{
   constexpr int DONE = 1;
   constexpr int CONTINUE = 2;
   constexpr int ERROR = -1;
   constexpr int SUCCESS = 0;

   class ParserInterface
   {
   public:
      /**
       * @brief Convert Y86-64 HCL code to C/Verilog code
       *
       * @param out The handler for the output
       * @return int Return 0 if success, -1 if error
       */
      virtual int toC(std::unique_ptr<CO::OutputInterface> &&out) = 0;
      virtual int toVerilog(std::unique_ptr<CO::OutputInterface> &&out) = 0;

   public:
      ParserInterface() = default;
      ParserInterface(const ParserInterface &) = delete;
      ParserInterface &operator=(const ParserInterface &) = delete;
      virtual ~ParserInterface() = default;
   };

}
