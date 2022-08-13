#pragma once

#include "./hcl_interface.h"
#include "./node.h"

#include <memory>
#include <vector>
#include <unordered_map>
#include <string>

namespace HCL
{
   class Parser : public ParserInterface
   {
   public:
      explicit Parser(std::unique_ptr<CO::InputInterface> &&in);
      int toC(std::unique_ptr<CO::OutputInterface> &&out) override;
      int toVerilog(std::unique_ptr<CO::OutputInterface> &&out) override;

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

      void insertCode(NodePtr quoteStr);
      void addArg(NodePtr var, NodePtr quoteStr, int isbool);
      void genFunct(NodePtr var, NodePtr expr, int isbool);
      NodePtr concat(NodePtr n1, NodePtr n2);

      void finishLine();
      int getLineNum() const;
      void fail(const char *format, ...);

   private:
      void checkArg(NodePtr arg, int wantbool);
      NodePtr findSymbol(char *name);

      const char *showExpr(NodePtr expr);
      void showExprHelper(NodePtr expr);
      void showTwoArgsExpr(NodePtr expr, const std::string &opStr);

   private:
      std::unique_ptr<CO::InputInterface> m_in;
      std::unique_ptr<CO::OutputInterface> m_out;

      int m_lineno;

      std::vector<Node> m_nodes;
      std::unordered_map<std::string, NodePtr> m_syms;

      // show expression members
      static const int MAX_SHOW_EXPR_LEN = 80;
      int m_exprBufLen = 0;
      char m_exprBuf[1024];
   };
}
