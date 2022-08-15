#include "./calc.h"

#include <assert.h>
#include <stdarg.h>

// function from yacc
extern void yyerror(Calc *, const char *);
extern int yyparse(Calc *);
// lexer input handler
extern FILE *yyin;

Calc::Calc(const char *fname)
{
   yyin = fopen(fname, "r");
}

Calc::~Calc()
{
   if (yyin != stdin)
      fclose(yyin);
}

void Calc::compute()
{
   yyparse(this);
}

void Calc::evalArithExpr(NodePtr expr)
{
   m_arithExprBuf.clear();
   double res = evalArithExprHelper(expr);
   printf("%s = %f\n", m_arithExprBuf.c_str(), res);
   m_nodes.clear();
}

NodePtr Calc::createNode(NodeType type, NodePtr left, NodePtr right, double num)
{
   m_nodes.emplace_back(std::make_shared<Node>(type, left, right, num));
   return m_nodes.back().get();
}

void Calc::fail(const char *format, ...)
{
   static char buffer[1024];
   va_list args;
   va_start(args, format);
   vsnprintf(buffer, sizeof(buffer), format, args);
   va_end(args);
   yyerror(this, buffer);
}

double Calc::evalArithExprHelper(NodePtr expr)
{

   for (const auto &node : m_nodes)
   {
      switch (node->type)
      {
      case t_ADD:
      {
         m_arithExprBuf.push_back('(');
         double l_val = evalArithExprHelper(node->left);
         m_arithExprBuf.push_back('+');
         double r_val = evalArithExprHelper(node->right);
         m_arithExprBuf.push_back(')');
         return l_val + r_val;
      }
      case t_SUB:
      {
         m_arithExprBuf.push_back('(');
         double l_val = evalArithExprHelper(node->left);
         m_arithExprBuf.push_back('-');
         double r_val = evalArithExprHelper(node->right);
         m_arithExprBuf.push_back(')');
         return l_val - r_val;
      }
      case t_MUL:
      {
         m_arithExprBuf.push_back('(');
         double l_val = evalArithExprHelper(node->left);
         m_arithExprBuf.push_back('*');
         double r_val = evalArithExprHelper(node->right);
         m_arithExprBuf.push_back(')');
         return l_val * r_val;
      }
      case t_DIV:
      {
         m_arithExprBuf.push_back('(');
         double l_val = evalArithExprHelper(node->left);
         m_arithExprBuf.push_back('/');
         double r_val = evalArithExprHelper(node->right);
         m_arithExprBuf.push_back(')');
         return l_val / r_val;
      }
      case t_MINUS:
      {
         m_arithExprBuf.push_back('(');
         m_arithExprBuf.push_back('-');
         double l_val = evalArithExprHelper(node->left);
         m_arithExprBuf.push_back(')');
         return -l_val;
      }
      case t_NUM:
      {
         std::string num = std::to_string(node->num);
         m_arithExprBuf.append(num);
         return node->num;
      }
      default:
         break;
      }
   }
   assert(0 && "Invalid Node Type");
   return 0;
}
