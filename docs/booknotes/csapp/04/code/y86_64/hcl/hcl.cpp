#include "./hcl.h"

#include <stdarg.h>

// function from yacc
extern void yyerror(HCL::Parser *par, const char *str);

namespace HCL
{
   Parser::Parser(std::unique_ptr<CO::InputInterface> &&in) : m_in(std::move(in)), m_lineno(0)
   {
   }

   int Parser::toC(std::unique_ptr<CO::OutputInterface> &&)
   {
      return 0;
   }

   int Parser::toVerilog(std::unique_ptr<CO::OutputInterface> &&)
   {
      return 0;
   }

   NodePtr Parser::createQuoteNode(const char *)
   {
      return nullptr;
   }

   NodePtr Parser::createSimpleNode(const char *)
   {
      return nullptr;
   }

   NodePtr Parser::createNotNode(NodePtr arg)
   {
      return nullptr;
   }

   NodePtr Parser::createAndNode(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createOrNode(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createCompNode(NodePtr op, NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createEleNode(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createCaseNode(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   void Parser::insertCode(NodePtr quoteStr)
   {
   }

   void Parser::addArg(NodePtr var, NodePtr quoteStr, int isbool)
   {
   }

   void Parser::genFunct(NodePtr var, NodePtr expr, int isbool)
   {
   }

   NodePtr Parser::concat(NodePtr n1, NodePtr n2)
   {
      return &m_nodes[0];
   }

   void Parser::finishLine()
   {
      m_lineno++;
   }

   int Parser::getLineNum() const
   {
      return m_lineno;
   }

   void Parser::fail(const char *format, ...)
   {
      static char buffer[1024];
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, sizeof(buffer), format, args);
      va_end(args);
      yyerror(this, buffer);
   }
}
