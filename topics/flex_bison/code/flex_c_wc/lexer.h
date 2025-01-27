#pragma once

class Lexer
{
public:
   explicit Lexer(const char *fname);
   ~Lexer();
   void count();

   void upCodeLineNum() { m_codeLineNum++; };
   void upCommentLineNum() { m_commentLineNum++; };
   void upEmptyLineNum() { m_emptyLineNum++; };
   void upIfNum() { m_ifNum++; }
   void upLoopNum() { m_loopNum++; }

private:
   void dump() const;

private:
   int m_codeLineNum = 0;
   int m_commentLineNum = 0;
   int m_emptyLineNum = 0;
   int m_ifNum = 0;
   int m_loopNum = 0;
};
