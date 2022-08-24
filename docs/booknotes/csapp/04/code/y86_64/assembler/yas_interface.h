#pragma once

#include "io_interface.h"

#include <memory>

namespace YAS
{
   constexpr int DONE = 1;
   constexpr int CONTINUE = 2;
   constexpr int ERROR = -1;
   constexpr int SUCCESS = 0;

   class LexerInterface
   {
   public:
      /**
       * @brief Convert Y86-64 assembly code to machine code
       *
       * @param out The handler for machine code output
       * @return int Return 0 if success, -1 if error
       */
      virtual int parse(std::shared_ptr<IO::OutputInterface> out) = 0;

   public:
      LexerInterface() = default;
      LexerInterface(const LexerInterface &) = delete;
      LexerInterface &operator=(const LexerInterface &) = delete;
      virtual ~LexerInterface() = default;
   };

}
