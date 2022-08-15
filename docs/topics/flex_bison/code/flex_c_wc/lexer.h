#pragma once

class Lexer
{
public:
   Lexer() = default;

   void upCodeLineNum() { m_codeLineNum++; };
   void upCommentLineNum() { m_commentLineNum++; };
   void upEmptyLineNum() { m_emptyLineNum++; };
   void upIfNum() { m_ifNum++; }
   void upLoopNum() { m_loopNum++; }

   void dump() const;

private:
   int m_codeLineNum = 0;
   int m_commentLineNum = 0;
   int m_emptyLineNum = 0;
   int m_ifNum = 0;
   int m_loopNum = 0;
};
