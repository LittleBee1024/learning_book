#include "./_common/input.h"

namespace IO
{
   FileIn::FileIn(const char *filename)
   {
      m_in = fopen(filename, "r");
   }

   FileIn::~FileIn()
   {
      if (m_in)
         fclose(m_in);
   }

   FILE *FileIn::getHandler()
   {
      return m_in;
   }

   MemIn::MemIn(const char *buf) : m_buf(buf)
   {
      // add new line character to support one line input
      m_buf.append("\n");
      m_in = fmemopen(&m_buf[0], m_buf.size(), "r");
   }

   MemIn::~MemIn()
   {
      if (m_in)
         fclose(m_in);
   }

   FILE *MemIn::getHandler()
   {
      return m_in;
   }
}
