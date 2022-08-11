#include "./input.h"

namespace YAS
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

   FILE *FileIn::getYasIn()
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

   FILE *MemIn::getYasIn()
   {
      return m_in;
   }

}
