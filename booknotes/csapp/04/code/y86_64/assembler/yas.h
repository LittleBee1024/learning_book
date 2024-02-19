#pragma once

#include "./assembler/yas_interface.h"
#include "./assembler/token.h"

#include <deque>
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>

namespace YAS
{

   class Lexer : public LexerInterface
   {
      friend class InstrLexer;

   public:
      explicit Lexer(std::unique_ptr<IO::InputInterface> &&in);
      int parse(std::shared_ptr<IO::OutputInterface> out) override;

   public:
      // save a line
      void saveLine(const char *);
      // save tokens from a line
      void addInstr(const char *);
      void addReg(const char *);
      void addNum(int64_t);
      void addPunct(char);
      void addIdent(const char *);
      // process a line after all tokens are added
      void processLine();
      // report errors for invalid tokens
      void fail(const char *);

   private:
      void reset();
      void cleanState();

   private:
      // process tokens from a line
      int processEmptyLine();
      int processLabel();
      int processPosInstr();
      int processAlignInstr();
      // process Y86-64 instruction needs ISA code
      int processNormalInstr();

      int findSymbol(const char *name);

      void dumpLine();
      void dumpCode(const std::vector<char> &decodeBuf = {});

   private:
      std::unique_ptr<IO::InputInterface> m_in;
      std::shared_ptr<IO::OutputInterface> m_out;

      int m_pass;
      bool m_hitError;
      std::unordered_map<std::string, int> m_symbols;

      int m_lineno;
      std::string m_line;
      std::deque<Token> m_tokens;
      bool m_lineError;

      int m_addr;
   };

}
