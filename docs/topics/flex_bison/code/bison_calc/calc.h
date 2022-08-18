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

typedef struct Node
{
public:
   Node(NodeType t, Node *l, Node *r, double n) : type(t), left(l), right(r), num(n) {}
   NodeType type;
   Node *left;
   Node *right;
   double num;
} *NodePtr;

class Calc
{
public:
   explicit Calc(const char *);
   ~Calc();
   void compute();

   NodePtr createNode(NodeType type, NodePtr left, NodePtr right, double num = 0);
   void evalExpr(NodePtr expr);
   void fail(const char *format, ...);

private:
   double evalExprHelper(NodePtr expr);

private:
   bool m_exprHasError = false;
   // store all nodes in the vector, so that they can be destroyed automatically
   std::vector<std::shared_ptr<Node>> m_nodes;
   std::string m_exprBuf;
};
