#include "./lexer.h"

#include <stdio.h>
#include <assert.h>

/*********************************
 * variable and function from flex
 * *******************************/
// lexer input handler
extern FILE *c_wcin;
// main lexer function generated by flex
extern int c_wclex(Lexer *lex);

Lexer::Lexer(const char *fname)
{
   c_wcin = fopen(fname, "r");
   assert(c_wcin != nullptr);
}

void Lexer::count()
{
   c_wclex(this);
   dump();
}

void Lexer::dump() const
{
   printf("The statistics of the source code:\n");
   printf("\tCode line number: %d, comment line number: %d, empty line number: %d\n",
      m_codeLineNum, m_commentLineNum, m_emptyLineNum);
   printf("\tIf number: %d, loop number: %d\n", m_ifNum, m_loopNum);
}
