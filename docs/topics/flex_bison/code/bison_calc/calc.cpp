#include "./calc.h"

#include <assert.h>
#include <stdarg.h>

// function from yacc
extern void yyerror(Calc *, const char *);
extern int yyparse(Calc *);
// lexer input handler
extern FILE *yyin;

Calc::Calc(const char *fname): m_exprHasError(false)
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
   if (!m_exprHasError)
   {
      double res = evalArithExprHelper(expr);
      printf("%s = %g\n", m_arithExprBuf.c_str(), res);
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

double Calc::evalArithExprHelper(NodePtr expr)
{
   switch (expr->type)
   {
   case t_ADD:
   {
      m_arithExprBuf.push_back('(');
      double l_val = evalArithExprHelper(expr->left);
      m_arithExprBuf.push_back('+');
      double r_val = evalArithExprHelper(expr->right);
      m_arithExprBuf.push_back(')');
      return l_val + r_val;
   }
   case t_SUB:
   {
      m_arithExprBuf.push_back('(');
      double l_val = evalArithExprHelper(expr->left);
      m_arithExprBuf.push_back('-');
      double r_val = evalArithExprHelper(expr->right);
      m_arithExprBuf.push_back(')');
      return l_val - r_val;
   }
   case t_MUL:
   {
      m_arithExprBuf.push_back('(');
      double l_val = evalArithExprHelper(expr->left);
      m_arithExprBuf.push_back('*');
      double r_val = evalArithExprHelper(expr->right);
      m_arithExprBuf.push_back(')');
      return l_val * r_val;
   }
   case t_DIV:
   {
      m_arithExprBuf.push_back('(');
      double l_val = evalArithExprHelper(expr->left);
      m_arithExprBuf.push_back('/');
      double r_val = evalArithExprHelper(expr->right);
      m_arithExprBuf.push_back(')');
      return l_val / r_val;
   }
   case t_MINUS:
   {
      m_arithExprBuf.push_back('(');
      m_arithExprBuf.push_back('-');
      double l_val = evalArithExprHelper(expr->left);
      m_arithExprBuf.push_back(')');
      return -l_val;
   }
   case t_NUM:
   {
      char buf[32];
      snprintf(buf, sizeof(buf), "%g", expr->num);
      m_arithExprBuf.append(buf);
      return expr->num;
   }
   default:
      break;
   }

   assert(0 && "Invalid Node Type");
   return 0;
}
