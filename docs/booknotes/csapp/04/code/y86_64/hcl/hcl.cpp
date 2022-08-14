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
      m_out = std::make_unique<OutGen>(std::move(out));
      m_outType = OutType::C;

      cleanState();
      yyparse(this);
      if (m_hitError)
         return ERROR;

      return SUCCESS;
   }

   int Parser::toVerilog(std::unique_ptr<CO::OutputInterface> &&out)
   {
      m_out = std::make_unique<OutGen>(std::move(out));
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
      if (!quote)
         return fail("Null Node");

      if (m_outType == OutType::Verilog)
         return;

      m_out->print(quote->sval.c_str());
      m_out->terminateLine();
   }

   void Parser::outExprC(NodePtr expr)
   {
      switch (expr->type)
      {
      case N_QUOTE:
      case N_COMP_OP:
      {
         return fail("Unexpected Node: %s", expr->sval.c_str());
         break;
      }
      case N_VAR:
      {
         NodePtr quote = refSymbol(expr->sval.c_str());
         if (!quote)
            return fail("Invalid variable '%s'", expr->sval.c_str());
         m_out->print("(%s)", quote->sval.c_str());
         break;
      }
      case N_NUM:
      {
         m_out->print(expr->sval.c_str());
         break;
      }
      case N_AND_EXPR:
      {
         m_out->print("(");
         outExprC(expr->arg1);
         m_out->print(" & ");
         outExprC(expr->arg2);
         m_out->print(")");
         break;
      }
      case N_OR_EXPR:
      {
         m_out->print("(");
         outExprC(expr->arg1);
         m_out->print(" | ");
         outExprC(expr->arg2);
         m_out->print(")");
         break;
      }
      case N_NOT_EXPR:
      {
         m_out->print("!");
         outExprC(expr->arg1);
         break;
      }
      case N_COMP_EXPR:
      {
         m_out->print("(");
         outExprC(expr->arg1);
         m_out->print(" %s ", expr->sval.c_str());
         outExprC(expr->arg2);
         m_out->print(")");
         break;
      }
      case N_ELE_EXPR:
      {
         m_out->print("(");
         for (NodePtr ele = expr->arg2; ele; ele = ele->next)
         {
            outExprC(expr->arg1);
            m_out->print(" == ");
            outExprC(ele);
            if (ele->next)
               m_out->print(" || ");
         }
         m_out->print(")");
         break;
      }
      case N_CASE_EXPR:
      {
         m_out->print("(");
         bool done = false;
         for (NodePtr ele = expr; ele && !done; ele = ele->next)
         {
            if (ele->arg1->type == N_NUM && atoll(ele->arg1->sval.c_str()) == 1)
            {
               outExprC(ele->arg2);
               done = true;
            }
            else
            {
               outExprC(ele->arg1);
               m_out->print(" ? ");
               outExprC(ele->arg2);
               m_out->print(" : ");
            }
         }
         if (!done)
            m_out->print("0");
         m_out->print(")");
         break;
      }
      default:
         break;
      }
   }

   void Parser::outFunction(NodePtr var, NodePtr expr, int isbool)
   {
      if (!var || !expr)
         return fail("Null Node");

      checkArg(expr, isbool);
      if (m_outType == OutType::Verilog)
      {
         return;
      }

      assert(m_outType == OutType::C);

      m_out->print("long long gen_%s()", var->sval.c_str());
      m_out->terminateLine();
      m_out->print("{");
      m_out->feedLineWithUpIndent();
      m_out->print("return ");
      outExprC(expr);
      m_out->print(";");
      m_out->feedLineWithDownIndent();
      m_out->print("}");
      m_out->terminateLine();
      m_out->terminateLine();

      return;
   }

   NodePtr Parser::createQuote(const char *quoteStr)
   {
      std::string sval(quoteStr);
      // remove first and laster quote character '
      sval = sval.substr(1, sval.size() - 2);
      m_nodes.emplace_back(std::make_shared<Node>(N_QUOTE, false, sval.c_str(), nullptr, nullptr));
      return m_nodes.back().get();
   }

   NodePtr Parser::createVar(const char *val)
   {
      m_nodes.emplace_back(std::make_shared<Node>(N_VAR, false, val, nullptr, nullptr));
      return m_nodes.back().get();
   }

   NodePtr Parser::createNum(const char *num)
   {
      m_nodes.emplace_back(std::make_shared<Node>(N_NUM, false, num, nullptr, nullptr));
      return m_nodes.back().get();
   }

   NodePtr Parser::createCompOp(const char *op)
   {
      m_nodes.emplace_back(std::make_shared<Node>(N_COMP_OP, false, op, nullptr, nullptr));
      return m_nodes.back().get();
   }

   NodePtr Parser::createAndExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, true);
      checkArg(arg2, true);
      m_nodes.emplace_back(std::make_shared<Node>(N_AND_EXPR, true, "&", arg1, arg2));
      return m_nodes.back().get();
   }

   NodePtr Parser::createOrExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, true);
      checkArg(arg2, true);
      m_nodes.emplace_back(std::make_shared<Node>(N_OR_EXPR, true, "|", arg1, arg2));
      return m_nodes.back().get();
   }

   NodePtr Parser::createNotExpr(NodePtr arg)
   {
      checkArg(arg, true);
      m_nodes.emplace_back(std::make_shared<Node>(N_NOT_EXPR, true, "!", arg, nullptr));
      return m_nodes.back().get();
   }

   NodePtr Parser::createCompExpr(NodePtr op, NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, false);
      checkArg(arg2, false);
      m_nodes.emplace_back(std::make_shared<Node>(N_COMP_EXPR, true, op->sval.c_str(), arg1, arg2));
      return m_nodes.back().get();
   }

   NodePtr Parser::createEleExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, false);
      for (NodePtr ele = arg1; ele; ele = ele->next)
         checkArg(ele, false);
      m_nodes.emplace_back(std::make_shared<Node>(N_ELE_EXPR, true, "in", arg1, arg2));
      return m_nodes.back().get();
   }

   NodePtr Parser::createCaseExpr(NodePtr arg1, NodePtr arg2)
   {
      checkArg(arg1, true);
      checkArg(arg2, false);
      m_nodes.emplace_back(std::make_shared<Node>(N_CASE_EXPR, false, ":", arg1, arg2));
      return m_nodes.back().get();
   }

   void Parser::setBool(NodePtr node)
   {
      if (!node)
         fail("Null node encountered");
      node->isbool = true;
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
         return;
      }

      if (arg->type == N_NUM)
      {
         if (wantbool && strcmp(arg->sval.c_str(), "0") != 0 && strcmp(arg->sval.c_str(), "1") != 0)
            return fail("Value '%s' not Boolean", arg->sval);
         return;
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
