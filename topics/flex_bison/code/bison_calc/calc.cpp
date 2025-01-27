#include "./calc.h"

#include <assert.h>
#include <stdarg.h>

// function from yacc
extern void calc_error(Calc *, const char *);
extern int calc_parse(Calc *);
// lexer input handler
extern FILE *calc_in;

Calc::Calc(const char *fname)
{
   calc_in = fopen(fname, "r");
}

Calc::~Calc()
{
   if (calc_in != stdin)
      fclose(calc_in);
}

void Calc::compute()
{
   calc_parse(this);
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
   calc_error(this, buffer);
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
