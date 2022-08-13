#include "./hcl.h"

#include <stdarg.h>
#include <string.h>

// function from yacc
extern void yyerror(HCL::Parser *par, const char *str);

namespace HCL
{
   Parser::Parser(std::unique_ptr<CO::InputInterface> &&in) : m_in(std::move(in)), m_lineno(0), m_exprBufLen(0)
   {
   }

   int Parser::toC(std::unique_ptr<CO::OutputInterface> &&out)
   {
      m_out = std::move(out);
      return 0;
   }

   int Parser::toVerilog(std::unique_ptr<CO::OutputInterface> &&out)
   {
      m_out = std::move(out);
      return 0;
   }

   NodePtr Parser::createQuote(const char *quoteStr)
   {
      std::string sval(quoteStr);
      // remove first and laster quote character '
      sval = sval.substr(1, sval.size() - 2);
      m_nodes.emplace_back(N_QUOTE, 0, sval.c_str(), nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createVar(const char *val)
   {
      m_nodes.emplace_back(N_VAR, 0, val, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createNum(const char *num)
   {
      m_nodes.emplace_back(N_NUM, 0, num, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createCompOp(const char *op)
   {
      m_nodes.emplace_back(N_COMP_OP, 0, op, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createAndExpr(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createOrExpr(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createNotExpr(NodePtr arg)
   {
      return nullptr;
   }

   NodePtr Parser::createCompExpr(NodePtr op, NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createEleExpr(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   NodePtr Parser::createCaseExpr(NodePtr arg1, NodePtr arg2)
   {
      return nullptr;
   }

   void Parser::insertCode(NodePtr quoteStr)
   {
   }

   void Parser::addArg(NodePtr var, NodePtr quoteStr, int isbool)
   {
   }

   void Parser::genFunct(NodePtr var, NodePtr expr, int isbool)
   {
   }

   NodePtr Parser::concat(NodePtr n1, NodePtr n2)
   {
      return &m_nodes[0];
   }

   void Parser::finishLine()
   {
      m_lineno++;
   }

   int Parser::getLineNum() const
   {
      return m_lineno;
   }

   void Parser::fail(const char *format, ...)
   {
      static char buffer[1024];
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, sizeof(buffer), format, args);
      va_end(args);
      yyerror(this, buffer);
   }

   void Parser::checkArg(HCL::NodePtr arg, int wantbool)
   {
      if (!arg)
         return fail("Null node encountered");

      if (arg->type == N_VAR)
      {
         auto iter = m_syms.find(arg->sval);
         if (iter == m_syms.end())
            return fail("Variable '%s' not found", arg->sval);
         if (wantbool != iter->second->isbool)
            return fail("Variable '%s' not %s", arg->sval, wantbool ? "Boolean" : "Integer");
      }

      if (arg->type == N_NUM)
      {
         if (wantbool && strcmp(arg->sval.c_str(), "0") != 0 && strcmp(arg->sval.c_str(), "1") != 0)
            return fail("Value '%s' not Boolean", arg->sval);
      }

      if (wantbool && !arg->isbool)
         return fail("Non Boolean argument '%s'", showExpr(arg));

      if (!wantbool && arg->isbool)
         return fail("Non integer argument '%s'", showExpr(arg));
   }

   void Parser::showTwoArgsExpr(NodePtr expr, const std::string &opStr)
   {
      sprintf(m_exprBuf + m_exprBufLen, "(");
      m_exprBufLen += 1;

      showExprHelper(expr->arg1);
      if (m_exprBufLen >= MAX_SHOW_EXPR_LEN)
         return;

      sprintf(m_exprBuf + m_exprBufLen, "%s", opStr.c_str());
      m_exprBufLen += opStr.length();

      showExprHelper(expr->arg2);

      sprintf(m_exprBuf + m_exprBufLen, ")");
      m_exprBufLen += 1;
   }

   void Parser::showExprHelper(NodePtr expr)
   {
      if (m_exprBufLen >= MAX_SHOW_EXPR_LEN)
         return;

      switch (expr->type)
      {
      case N_QUOTE:
      {
         int len = expr->sval.length() + 2;
         sprintf(m_exprBuf + m_exprBufLen, "'%s'", expr->sval.c_str());
         m_exprBufLen += len;
         break;
      }
      case N_VAR:
      case N_NUM:
      case N_COMP_OP:
      {
         int len = expr->sval.length();
         sprintf(m_exprBuf + m_exprBufLen, "%s", expr->sval.c_str());
         m_exprBufLen += len;
         break;
      }
      case N_AND_EXPR:
      {
         showTwoArgsExpr(expr, " & ");
         break;
      }
      case N_OR_EXPR:
      {
         showTwoArgsExpr(expr, " | ");
         break;
      }
      case N_NOT_EXPR:
      {
         sprintf(m_exprBuf + m_exprBufLen, "!");
         m_exprBufLen += 1;
         showExprHelper(expr->arg1);
         break;
      }
      case N_COMP_EXPR:
      {
         std::string opStr = " " + expr->sval + " ";
         showTwoArgsExpr(expr, opStr);
         break;
      }
      case N_ELE_EXPR:
      {
         sprintf(m_exprBuf + m_exprBufLen, "(");
         m_exprBufLen += 1;
         showExprHelper(expr->arg1);
         sprintf(m_exprBuf + m_exprBufLen, " in {");
         m_exprBufLen += 5;

         for (NodePtr ele = expr->arg2; m_exprBufLen < MAX_SHOW_EXPR_LEN && ele; ele = ele->next)
         {
            showExprHelper(ele);
            if (ele->next)
            {
               sprintf(m_exprBuf + m_exprBufLen, ", ");
               m_exprBufLen += 2;
            }
         }

         if (m_exprBufLen >= MAX_SHOW_EXPR_LEN)
            return;
         sprintf(m_exprBuf + m_exprBufLen, "})");
         m_exprBufLen += 2;
         break;
      }
      case N_CASE_EXPR:
      {
         sprintf(m_exprBuf + m_exprBufLen, "[ ");
         m_exprBufLen += 2;

         for (NodePtr ele = expr; m_exprBufLen < MAX_SHOW_EXPR_LEN && ele; ele = ele->next)
         {
            showExprHelper(ele->arg1);
            sprintf(m_exprBuf + m_exprBufLen, " : ");
            m_exprBufLen += 3;
            showExprHelper(ele->arg2);
         }

         if (m_exprBufLen >= MAX_SHOW_EXPR_LEN)
            return;
         sprintf(m_exprBuf + m_exprBufLen, " ]");
         m_exprBufLen += 2;
         break;
      }
      default:
      {
         sprintf(m_exprBuf + m_exprBufLen, "??");
         m_exprBufLen += 2;
         break;
      }
      }
   }

   const char *Parser::showExpr(NodePtr expr)
   {
      m_exprBufLen = 0; // clear buffer
      showExprHelper(expr);
      if (m_exprBufLen >= MAX_SHOW_EXPR_LEN)
         sprintf(m_exprBuf + m_exprBufLen, "...");
      return m_exprBuf;
   }
}
