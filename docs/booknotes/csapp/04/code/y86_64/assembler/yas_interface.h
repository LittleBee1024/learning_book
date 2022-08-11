#pragma once

#include <memory>

namespace YAS
{
   constexpr int DONE = 1;
   constexpr int CONTINUE = 2;
   constexpr int ERROR = -1;
   constexpr int SUCCESS = 0;

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

   class LexerInterface
   {
   public:
      /**
       * @brief Convert Y86-64 assembly code to machine code
       *
       * @param out The handler for machine code output
       * @return int Return 0 if success, -1 if error
       */
      virtual int parse(std::unique_ptr<OutputInterface> &&out) = 0;

   public:
      LexerInterface() = default;
      LexerInterface(const LexerInterface &) = delete;
      LexerInterface &operator=(const LexerInterface &) = delete;
      virtual ~LexerInterface() = default;
   };

}
