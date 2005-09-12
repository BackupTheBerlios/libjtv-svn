#include <string.h>

extern "C" char *strnew (const char *instr)
{
  if (!instr)
    return NULL;
  size_t sl = strlen (instr);
  char *tmp = new char [sl + 1];
  memcpy (tmp, instr, sl + 1);
  return tmp;
}
