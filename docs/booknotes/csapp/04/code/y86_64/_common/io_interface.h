#pragma once

#include <stdio.h>

namespace CO
{
   class InputInterface
   {
   public:
      /**
       * @brief Get the handler of flex input
       */
      virtual FILE *getYasIn() = 0;

   public:
      InputInterface() = default;
      InputInterface(const InputInterface &) = delete;
      InputInterface &operator=(const InputInterface &) = delete;
      virtual ~InputInterface() = default;
   };

   class OutputInterface
   {
   public:
      /**
       * @brief Output string
       *
       * @param format The format of the string
       */
      virtual void out(const char *format, ...) = 0;

   public:
      OutputInterface() = default;
      OutputInterface(const OutputInterface &) = delete;
      OutputInterface &operator=(const OutputInterface &) = delete;
      virtual ~OutputInterface() = default;
   };
}
