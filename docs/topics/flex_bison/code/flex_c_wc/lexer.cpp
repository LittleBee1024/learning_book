#include "./lexer.h"

#include <stdio.h>

void Lexer::dump() const
{
   printf("The statistics of the source code:\n");
   printf("\tCode line number: %d, comment line number: %d, empty line number: %d\n",
      m_codeLineNum, m_commentLineNum, m_emptyLineNum);
   printf("\tIf number: %d, loop number: %d\n", m_ifNum, m_loopNum);
}
