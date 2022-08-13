#include "./hcl.h"

#include <stdarg.h>
#include <string>

// function from yacc
extern void yyerror(HCL::Parser *par, const char *str);

namespace HCL
{
   Parser::Parser(std::unique_ptr<CO::InputInterface> &&in) : m_in(std::move(in)), m_lineno(0)
   {
   }

   int Parser::toC(std::unique_ptr<CO::OutputInterface> &&out)
   {
      m_out = std::move(out);
      return 0;
   }

   int Parser::toVerilog(std::unique_ptr<CO::OutputInterface> &&out)
   {
      m_out = std::move(out);
      return 0;
   }

   NodePtr Parser::createQuote(const char *quoteStr)
   {
      std::string sval(quoteStr);
      // remove first and laster quote character '
      sval = sval.substr(1, sval.size() - 2);
      m_nodes.emplace_back(N_QUOTE, 0, sval.c_str(), nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createVar(const char *val)
   {
      m_nodes.emplace_back(N_VAR, 0, val, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createNum(const char *num)
   {
      m_nodes.emplace_back(N_NUM, 0, num, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createCompOp(const char *op)
   {
      m_nodes.emplace_back(N_COMP_OP, 0, op, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createAndExpr(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createOrExpr(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createNotExpr(NodePtr arg)
   {
      return nullptr;
   }

   NodePtr Parser::createCompExpr(NodePtr op, NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createEleExpr(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createCaseExpr(NodePtr arg1, NodePtr arg2)
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

   void Parser::checkArg(HCL::NodePtr arg, int wantbool)
   {
      if (!arg)
         return fail("Null node encountered");
   }
}
