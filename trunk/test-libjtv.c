#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <langinfo.h>
#include <iconv.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include "libjtv.h"
#include "strnew.h"

int main(int argc, char *argv[])
{
  if (argc != 2)
  {
   printf("Usage : test-jtv file_tvprog.zip\n");
   return 0;
  }

  ch_alias_list *chl = LoadChannelAliasList(CHANNEL_ALIAS_LIST);

  setlocale(LC_ALL,"");
  char *date_format = strnew(nl_langinfo(D_FMT));
  char *time_format = strnew(nl_langinfo(T_FMT));

  iconv_t cnv_content = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                                   chl->cp_content);
  if (cnv_content == (iconv_t) -1)
  {
    printf("Iconv open return %d error code\n", errno);
    return errno;
  }

  iconv_t cnv_zip_fn = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                                  chl->cp_zip_fn);
  if (cnv_zip_fn == (iconv_t) -1)
  {
    printf("Iconv open return %d error code\n", errno);
    return errno;
  }

  tv_list *tvl = LoadJTV(argv[1],CHANNEL_ALIAS_LIST, 0);
  unsigned int i;
  int cur_day = -1;
  char *cur_ch = "";

  for (i = 0; i < tvl->num; i++)
  {
    struct tm *tmp;
    char *pn;
    tmp = localtime(&tvl->tvp[i].time);
    if (strcmp(tvl->tvp[i].ch_name, cur_ch) != 0)
    {
      cur_ch = tvl->tvp[i].ch_name;
      //printf("Channel %s : ", strnewcnv(cnv_zip_fn,cur_ch));
      printf("Channel %s : ", cur_ch);
    }
#if 1
    char tbuf[100];
    if (cur_day != tmp->tm_mday)
    {
      strftime(tbuf,100,date_format,tmp);
      printf("%s\n", tbuf);
      cur_day = tmp->tm_mday;
    }

    strftime(tbuf,100,time_format,tmp);
    pn = strnewcnv(cnv_content, tvl->tvp[i].prg_name);
    printf("%s %s\n",
           tbuf,pn);
    free(pn);
#endif
  }
  free(date_format);free(time_format);
  iconv_close(cnv_zip_fn);
  iconv_close(cnv_content);
  FreeJTV(tvl);
}
