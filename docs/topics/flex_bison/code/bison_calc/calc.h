#pragma once

#include <vector>
#include <memory>
#include <string>

enum NodeType : int
{
   t_ADD,
   t_SUB,
   t_MUL,
   t_DIV,
   t_NUM,
   t_MINUS
};

class Node;
typedef Node *NodePtr;
class Node
{
public:
   Node(NodeType t, NodePtr l, NodePtr r, double n) : type(t), left(l), right(r), num(n) {}
   NodeType type;
   NodePtr left;
   NodePtr right;
   double num;
};

class Calc
{
public:
   explicit Calc(const char *);
   ~Calc();
   void compute();

   NodePtr createNode(NodeType type, NodePtr left, NodePtr right, double num = 0);
   void evalArithExpr(NodePtr expr);
   void fail(const char *format, ...);

private:
   double evalArithExprHelper(NodePtr expr);

private:
   // store all nodes in the vector, so that they can be destroyed automatically
   std::vector<std::shared_ptr<Node>> m_nodes;
   std::string m_arithExprBuf;
};
