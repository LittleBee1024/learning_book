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

void Calc::evalExpr(NodePtr expr)
{
   m_exprBuf.clear();
   if (!m_exprHasError)
   {
      double res = evalExprHelper(expr);
      printf("%s = %g\n", m_exprBuf.c_str(), res);
   }
   m_exprHasError = false;
   m_nodes.clear();
}

NodePtr Calc::createNode(NodeType type, NodePtr left, NodePtr right, double num)
{
   m_nodes.emplace_back(std::make_shared<Node>(type, left, right, num));
   return m_nodes.back().get();
}

void Calc::fail(const char *format, ...)
{
   m_exprHasError = true;
   static char buffer[1024];
   va_list args;
   va_start(args, format);
   vsnprintf(buffer, sizeof(buffer), format, args);
   va_end(args);
   yyerror(this, buffer);
}

double Calc::evalExprHelper(NodePtr expr)
{
   switch (expr->type)
   {
   case t_ADD:
   {
      m_exprBuf.push_back('(');
      double l_val = evalExprHelper(expr->left);
      m_exprBuf.push_back('+');
      double r_val = evalExprHelper(expr->right);
      m_exprBuf.push_back(')');
      return l_val + r_val;
   }
   case t_SUB:
   {
      m_exprBuf.push_back('(');
      double l_val = evalExprHelper(expr->left);
      m_exprBuf.push_back('-');
      double r_val = evalExprHelper(expr->right);
      m_exprBuf.push_back(')');
      return l_val - r_val;
   }
   case t_MUL:
   {
      m_exprBuf.push_back('(');
      double l_val = evalExprHelper(expr->left);
      m_exprBuf.push_back('*');
      double r_val = evalExprHelper(expr->right);
      m_exprBuf.push_back(')');
      return l_val * r_val;
   }
   case t_DIV:
   {
      m_exprBuf.push_back('(');
      double l_val = evalExprHelper(expr->left);
      m_exprBuf.push_back('/');
      double r_val = evalExprHelper(expr->right);
      m_exprBuf.push_back(')');
      return l_val / r_val;
   }
   case t_MINUS:
   {
      m_exprBuf.push_back('(');
      m_exprBuf.push_back('-');
      double l_val = evalExprHelper(expr->left);
      m_exprBuf.push_back(')');
      return -l_val;
   }
   case t_NUM:
   {
      char buf[32];
      snprintf(buf, sizeof(buf), "%g", expr->num);
      m_exprBuf.append(buf);
      return expr->num;
   }
   default:
      break;
   }

   assert(0 && "Invalid Node Type");
   return 0;
}
