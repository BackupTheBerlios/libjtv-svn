#include <string.h>
#include <stdlib.h>

extern "C" char *strnew (const char *instr)
{
  if (!instr)
    return NULL;
  size_t sl = strlen (instr);
  char *tmp = (char*) malloc (sl + 1);
  memcpy (tmp, instr, sl + 1);
  return tmp;
}
