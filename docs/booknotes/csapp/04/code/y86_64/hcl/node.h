#pragma once

#include <cstdint>
#include <string>

namespace HCL
{
   enum NodeType : int
   {
      N_QUOTE,    // quote string which starts/ends with ' character
      N_VAR,      // variable name
      N_NUM,      // number
      N_COMP_OP,  // compare operator [COMP_OP]
      N_AND_EXPR, // and expression, node && node
      N_OR_EXPR,  // or expression, node || node
      N_NOT_EXPR, // not expression, !node
      N_COMP_EXPR,// compare expression, node [COMP_OP] node
      N_ELE_EXPR, // HCL in expression, expr in exprlist
      N_CASE_EXPR // HCL case expression, expr : expr
   };

   class Node;
   typedef Node *NodePtr;
   class Node
   {
   public:
      Node(NodeType t, int b, const char *s, NodePtr a1, NodePtr a2)
          : type(t), isbool(b), sval(s), arg1(a1), arg2(a2), ref(0), next(nullptr)
      {
      }

      NodeType type;
      int isbool;       // Is this node a Boolean expression
      std::string sval; // Node contents
      Node *arg1;
      Node *arg2;
      int ref; // For var, how many times has it been referenced
      Node *next;
   };
}
