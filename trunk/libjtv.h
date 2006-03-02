#include <time.h>
typedef struct {
  char *ch_name;
  char *prg_name;
  int ch_index;
  time_t time;
  time_t etime;
  int index_time;  // sort index by time  (fills not lib, but application)
  int index_etime; // sort index by etime
//  int tm_isdst;
} tv_program;

typedef struct {
  unsigned int num;
  tv_program *tvp;
} tv_list;

typedef struct {
  char *zip_name;
  char *real_name;
} ch_alias;

#define CP_ZIP_FN_ALLOC 1
#define CP_CONTENT_ALLOC 2

typedef struct {
  unsigned int num;
  char *cp_zip_fn; // codepage for zip filenames
  char *cp_content; // codepage for content of files
  char cp_flags; // for check allocation codepage strings
  ch_alias cha[];
} ch_alias_list;

#ifdef __cplusplus
extern "C" tv_list * LoadJTV(char *fname, char *ch_alias_name, int correctTZ, char *cp_zin_fn, char *cp_content, ch_alias_list **out_chl);
extern "C" void FreeJTV(tv_list *tvl);
extern "C" ch_alias_list *LoadChannelAliasList(char *fname, char *cp_zin_fn, char *cp_content);
extern "C" void FreeChannelAliasList(ch_alias_list *ch_list);
extern "C" char *strnewcnv(iconv_t cnv, char *str);
#else
extern tv_list * LoadJTV(char *fname, char *ch_alias_name, int correctTZ, char *cp_zin_fn, char *cp_content, ch_alias_list **out_chl);
extern void FreeJTV(tv_list *tvl);
extern ch_alias_list *LoadChannelAliasList(char *fname, char *cp_zin_fn, char *cp_content);
extern void FreeChannelAliasList(ch_alias_list *ch_list);
extern char *strnewcnv(iconv_t cnv, char *str);
#endif

#define CHANNEL_ALIAS_LIST "channel.alias.rc"
