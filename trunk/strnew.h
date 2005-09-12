#ifndef __STRNEW_H__
#define __STRNEW_H__

#if defined (OS_WIN32)
#  ifndef MAXPATHLEN
#    define MAXPATHLEN          _MAX_PATH
#  endif
#  define PATH_SEPARATOR        '\\'
#  define PATH_SEPARATOR_S      "\\"
#  define PATH_SEPARATOR_2      '/'
#  define PATH_SEPARATOR_2_S    "/"
#  define DRIVE_SEPARATOR       ':'
#elif defined (OS_LINUX)
#  define MOUNT_PREFIX          "/mnt/"
#  define MAXPATHLEN            1024
#  define PATH_SEPARATOR        '/'
#  define PATH_SEPARATOR_S      "/"
#  define PATH_SEPARATOR_2      PATH_SEPARATOR
#  define PATH_SEPARATOR_2_S    PATH_SEPARATOR_S
#else
#  error "Unknown operating system!"
#endif

#if defined (OS_WIN32) || defined (OS_OS2)
#  define TEMP_DIR ""
#  ifdef _MSC_VER
#    include <io.h>
#  else
#    include <unistd.h>
#  endif
#  include <process.h>
#  define TEMP_FILE "%lud.tmp", (unsigned long)getpid()
#elif defined (OS_LINUX)
#  define TEMP_DIR "/tmp/"
#  define TEMP_FILE "%lud.tmp", (unsigned long)getpid()
#  include <unistd.h>
#endif

#define APPEND_SLASH(str,len)                   \
   if ((len)                                    \
    && (str[len - 1] != '/')                    \
    && (str[len - 1] != PATH_SEPARATOR))        \
   {                                            \
     str[len++] = PATH_SEPARATOR;               \
     str[len] = 0;                              \
   } /* endif */


#include <string.h>
#ifdef __cplusplus
extern "C" char *strnew (const char *instr);
#else
extern char *strnew (const char *instr);
#endif
#endif // __STRNEW_H__
