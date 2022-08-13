#pragma once

#include "./hcl_interface.h"
#include "./node.h"

#include <memory>
#include <vector>

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

   private:
      std::unique_ptr<CO::InputInterface> m_in;
      std::unique_ptr<CO::OutputInterface> m_out;

      int m_lineno;

      std::vector<Node> m_nodes;
   };
}
