#include "./_common/output.h"

#include <stdio.h>
#include <stdarg.h>

namespace IO
{
   FileOut::FileOut(const char *filename) : m_out(filename)
   {
   }

   FileOut::~FileOut()
   {
      m_out.close();
   }

   void FileOut::out(const char *format, ...)
   {
      static char buffer[1024];
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, sizeof(buffer), format, args);
      va_end(args);
      m_out << buffer;
      m_out << std::flush;
   }

   void MemOut::out(const char *format, ...)
   {
      static char buffer[1024];
      va_list args;
      va_start(args, format);
      vsnprintf(buffer, sizeof(buffer), format, args);
      va_end(args);
      m_out << buffer;
   }

   std::string MemOut::dump() const
   {
      return m_out.str();
   }

   void StdOut::out(const char *format, ...)
   {
      va_list args;
      va_start(args, format);
      vprintf(format, args);
      va_end(args);
   }
}
