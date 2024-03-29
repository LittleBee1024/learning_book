#pragma once

#include "./hcl/hcl_interface.h"
#include "./hcl/node.h"
#include "./hcl/outgen.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace HCL
{
   class Parser : public ParserInterface
   {
   public:
      explicit Parser(std::unique_ptr<IO::InputInterface> &&in);
      int toC(std::shared_ptr<IO::OutputInterface> out) override;
      int toVerilog(std::shared_ptr<IO::OutputInterface> out) override;

   public:
      void outFunction(NodePtr var, NodePtr expr, int isbool);
      void outQuoteCode(NodePtr quote);

   public:
      NodePtr createQuote(const char *quoteStr);
      NodePtr createVar(const char *var);
      NodePtr createNum(const char *num);
      NodePtr createCompOp(const char *op);
      NodePtr createAndExpr(NodePtr arg1, NodePtr arg2);
      NodePtr createOrExpr(NodePtr arg1, NodePtr arg2);
      NodePtr createNotExpr(NodePtr arg);
      NodePtr createCompExpr(NodePtr op, NodePtr arg1, NodePtr arg2);
      NodePtr createEleExpr(NodePtr arg1, NodePtr arg2);
      NodePtr createCaseExpr(NodePtr arg1, NodePtr arg2);

      void addSymbol(NodePtr var, NodePtr quote, int isbool);
      NodePtr concat(NodePtr n1, NodePtr n2);

      void finishLine();
      int getLineNum() const;
      void fail(const char *format, ...);

   private:
      void cleanState();
      void checkArg(NodePtr arg, bool wantbool);
      NodePtr refSymbol(const char *varName);
      void setBool(NodePtr node);

      const char *showExpr(NodePtr expr);
      void showExprHelper(NodePtr expr);
      void showTwoArgsExpr(NodePtr expr, const std::string &opStr);
      void outExprHelper(NodePtr expr);

      void outCFunction(NodePtr var, NodePtr expr, int isbool);
      void outVerilogFunction(NodePtr var, NodePtr expr, int isbool);

   private:
      std::unique_ptr<IO::InputInterface> m_in;
      std::unique_ptr<OutGen> m_out;

      int m_lineno;
      bool m_hitError;
      enum class OutType
      {
         C,
         Verilog
      };
      OutType m_outType;

      // store all nodes in the vector
      std::vector<std::shared_ptr<Node>> m_nodes;

      // signals used in HCL are stored as symbols, for example,
      // boolean signal: boolsig s1 's1_val'
      // non-bool signal: wordsig code 'code_val'
      static const int MAX_SYM_NUM = 100;
      // the key is signal string in N_VAR node, the value is N_QUOTE node of the N_VAR node
      std::unordered_map<std::string, NodePtr> m_syms;

      // show expression members
      static const int MAX_SHOW_EXPR_LEN = 80;
      std::string m_exprBuf;
   };
}
