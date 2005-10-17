#include <time.h>
typedef struct {
  char *ch_name;
  char *prg_name;
  int ch_index;
  time_t time;
} tv_program;

typedef struct {
  unsigned int num;
  tv_program *tvp;
} tv_list;

typedef struct {
  char *zip_name;
  char *real_name;
} ch_alias;

typedef struct {
  unsigned int num;
  char *cp_zip_fn; // codepage for zip filenames
  char *cp_content; // codepage for content of files
  ch_alias cha[];
} ch_alias_list;

#ifdef __cplusplus
extern "C" tv_list * LoadJTV(char *fname, char *ch_alias_name);
extern "C" void FreeJTV(tv_list *tvl);
extern "C" ch_alias_list *LoadChannelAliasList(char *fname);
extern "C" void FreeChannelAliasList(ch_alias_list *ch_list);
extern "C" char *strnewcnv(iconv_t cnv, char *str);
#else
extern tv_list * LoadJTV(char *fname, char *ch_alias_name);
extern void FreeJTV(tv_list *tvl);
extern ch_alias_list *LoadChannelAliasList(char *fname);
extern void FreeChannelAliasList(ch_alias_list *ch_list);
extern char *strnewcnv(iconv_t cnv, char *str);
#endif

#define CHANNEL_ALIAS_LIST "channel.alias.rc"
