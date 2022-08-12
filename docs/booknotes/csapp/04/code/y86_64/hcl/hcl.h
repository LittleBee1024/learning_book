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
      NodePtr createQuoteNode(const char *);
      NodePtr createSimpleNode(const char *);
      NodePtr createNotNode(NodePtr arg);
      NodePtr createAndNode(NodePtr arg1, NodePtr arg2);
      NodePtr createOrNode(NodePtr arg1, NodePtr arg2);
      NodePtr createCompNode(NodePtr op, NodePtr arg1, NodePtr arg2);
      NodePtr createEleNode(NodePtr arg1, NodePtr arg2);
      NodePtr createCaseNode(NodePtr arg1, NodePtr arg2);

      void insertCode(NodePtr quoteStr);
      void addArg(NodePtr var, NodePtr quoteStr, int isbool);
      void genFunct(NodePtr var, NodePtr expr, int isbool);
      NodePtr concat(NodePtr n1, NodePtr n2);

      void finishLine();

   private:
      std::unique_ptr<CO::InputInterface> m_in;
      std::unique_ptr<CO::OutputInterface> m_out;

      int m_lineno;

      std::vector<Node> m_nodes;
   };
}
