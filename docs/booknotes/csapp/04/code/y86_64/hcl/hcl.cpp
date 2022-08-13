#include "./hcl.h"

#include <stdarg.h>
#include <string.h>
#include <cassert>

// function from yacc
extern void yyerror(HCL::Parser *par, const char *str);
extern int yyparse(HCL::Parser *par);
// lexer input handler
extern FILE *yyin;

namespace HCL
{
   Parser::Parser(std::unique_ptr<CO::InputInterface> &&in) : m_in(std::move(in)), m_lineno(0), m_hitError(false), m_outType(OutType::C)
   {
      // yyin is a global variable defined in flex
      assert(m_in->getHandler() != nullptr);
      yyin = m_in->getHandler();
   }

   int Parser::toC(std::unique_ptr<CO::OutputInterface> &&out)
   {
      m_out = std::move(out);
      m_outType = OutType::C;

      cleanState();
      yyparse(this);
      if (m_hitError)
         return ERROR;

      return SUCCESS;
   }

   int Parser::toVerilog(std::unique_ptr<CO::OutputInterface> &&out)
   {
      m_out = std::move(out);
      m_outType = OutType::Verilog;

      cleanState();
      yyparse(this);
      if (m_hitError)
         return ERROR;

      return SUCCESS;
   }

   void Parser::cleanState()
   {
      m_lineno = 0;
      m_hitError = false;
      fseek(yyin, 0, SEEK_SET);
   }

   void Parser::outQuoteCode(NodePtr quote)
   {
   }

   void Parser::outFunction(NodePtr var, NodePtr expr, int isbool)
   {
   }

   NodePtr Parser::createQuote(const char *quoteStr)
   {
      std::string sval(quoteStr);
      // remove first and laster quote character '
      sval = sval.substr(1, sval.size() - 2);
      m_nodes.emplace_back(N_QUOTE, false, sval.c_str(), nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createVar(const char *val)
   {
      m_nodes.emplace_back(N_VAR, false, val, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createNum(const char *num)
   {
      m_nodes.emplace_back(N_NUM, false, num, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createCompOp(const char *op)
   {
      m_nodes.emplace_back(N_COMP_OP, false, op, nullptr, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createAndExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, true);
      checkArg(arg2, true);
      m_nodes.emplace_back(N_AND_EXPR, true, "&", arg1, arg2);
      return &m_nodes.back();
   }

   NodePtr Parser::createOrExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, true);
      checkArg(arg2, true);
      m_nodes.emplace_back(N_OR_EXPR, true, "|", arg1, arg2);
      return &m_nodes.back();
   }

   NodePtr Parser::createNotExpr(NodePtr arg)
   {
      checkArg(arg, true);
      m_nodes.emplace_back(N_NOT_EXPR, true, "!", arg, nullptr);
      return &m_nodes.back();
   }

   NodePtr Parser::createCompExpr(NodePtr op, NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, false);
      checkArg(arg2, false);
      m_nodes.emplace_back(N_COMP_EXPR, true, op->sval.c_str(), arg1, arg2);
      return &m_nodes.back();
   }

   NodePtr Parser::createEleExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, false);
      for (NodePtr ele = arg1; ele; ele = ele->next)
         checkArg(ele, false);
      m_nodes.emplace_back(N_ELE_EXPR, true, "in", arg1, arg2);
      return &m_nodes.back();
   }

   NodePtr Parser::createCaseExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, true);
      checkArg(arg2, false);
      m_nodes.emplace_back(N_CASE_EXPR, false, ":", arg1, arg2);
      return &m_nodes.back();
   }

   void Parser::setBool(NodePtr node)
   {
      if (!node)
         fail("Null node encountered");
      node->isbool = true;
   }

   void Parser::addSymbol(NodePtr var, NodePtr quote, int isbool)
   {
      if (!var || !quote)
         return fail("Null node");

      m_syms.emplace(var->sval, quote);
      if (isbool)
      {
         setBool(var);
         setBool(quote);
      }
   }

   NodePtr Parser::concat(NodePtr n1, NodePtr n2)
   {
      if (!n1)
         return n2;

      NodePtr tail = n1;
      while (tail->next)
         tail = tail->next;
      tail->next = n2;
      return n1;
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
      m_hitError = true;
      static char buffer[1024];
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, sizeof(buffer), format, args);
      va_end(args);
      yyerror(this, buffer);
   }

   NodePtr Parser::refSymbol(const char *varName)
   {
      auto iter = m_syms.find(varName);
      if (iter == m_syms.end())
         return nullptr;
      NodePtr quote = iter->second;
      quote->ref++;
      return quote;
   }

   void Parser::checkArg(HCL::NodePtr arg, bool wantbool)
   {
      if (!arg)
         return fail("Null node encountered");

      if (arg->type == N_VAR)
      {
         NodePtr quote = refSymbol(arg->sval.c_str());
         if (!quote)
            return fail("Variable '%s' not found", arg->sval);
         if (wantbool != quote->isbool)
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
      m_exprBuf += "(";
      showExprHelper(expr->arg1);

      if (m_exprBuf.length() >= MAX_SHOW_EXPR_LEN)
         return;

      m_exprBuf += opStr;
      showExprHelper(expr->arg2);
      m_exprBuf += ")";
   }

   void Parser::showExprHelper(NodePtr expr)
   {
      if (m_exprBuf.length() >= MAX_SHOW_EXPR_LEN)
         return;

      switch (expr->type)
      {
      case N_QUOTE:
      {
         m_exprBuf += "'" + expr->sval + "'";
         break;
      }
      case N_VAR:
      case N_NUM:
      case N_COMP_OP:
      {
         m_exprBuf += expr->sval;
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
         m_exprBuf += "!";
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
         m_exprBuf += "(";
         showExprHelper(expr->arg1);
         m_exprBuf += " in {";

         for (NodePtr ele = expr->arg2; m_exprBuf.length() < MAX_SHOW_EXPR_LEN && ele; ele = ele->next)
         {
            showExprHelper(ele);
            if (ele->next)
               m_exprBuf += ", ";
         }

         if (m_exprBuf.length() >= MAX_SHOW_EXPR_LEN)
            return;

         m_exprBuf += "})";
         break;
      }
      case N_CASE_EXPR:
      {
         m_exprBuf += "[ ";

         for (NodePtr ele = expr; m_exprBuf.length() < MAX_SHOW_EXPR_LEN && ele; ele = ele->next)
         {
            showExprHelper(ele->arg1);
            m_exprBuf += " : ";
            showExprHelper(ele->arg2);
         }

         if (m_exprBuf.length() >= MAX_SHOW_EXPR_LEN)
            return;

         m_exprBuf += " ]";
         break;
      }
      default:
      {
         m_exprBuf += "??";
         break;
      }
      }
   }

   const char *Parser::showExpr(NodePtr expr)
   {
      m_exprBuf.clear();

      showExprHelper(expr);
      if (m_exprBuf.length() >= MAX_SHOW_EXPR_LEN)
         m_exprBuf += "...";

      return m_exprBuf.c_str();
   }
}