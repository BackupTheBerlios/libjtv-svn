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
  int ret = 0;

  if (argc != 2)
  {
   printf("Usage : test-jtv file_tvprog.zip\n");
   return ret;
  }

  ch_alias_list *chl;
  unsigned int i;
  int cur_day = -1;
  char *cur_ch = "";
  iconv_t cnv_zip_fn, cnv_content;

  //= LoadChannelAliasList(CHANNEL_ALIAS_LIST, NULL, NULL);

  setlocale(LC_ALL,"");
  char *date_format = strnew(nl_langinfo(D_FMT));
  char *time_format = strnew(nl_langinfo(T_FMT));

  tv_list *tvl = LoadJTV(argv[1],CHANNEL_ALIAS_LIST, 0, NULL, NULL, &chl);

  cnv_content = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                                   chl->cp_content);
  if (cnv_content == (iconv_t) -1)
  {
    printf("Iconv open return %d error code\n", errno);
    ret = errno;
    goto g_free_jtv;
  }

  cnv_zip_fn = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                                  chl->cp_zip_fn);
  if (cnv_zip_fn == (iconv_t) -1)
  {
    printf("Iconv open return %d error code\n", errno);
    ret = errno;
    goto g_free_content;
  }

  for (i = 0; i < tvl->num; i++)
  {
    struct tm *tmp;
    char *pn;
    tmp = localtime(&tvl->tvp[i].time);
    if (strcmp(tvl->tvp[i].ch_name, cur_ch) != 0)
    {
      cur_ch = tvl->tvp[i].ch_name;
      //printf("Channel %s : ", strnewcnv(cnv_zip_fn,cur_ch));
      printf("Channel %s\n", cur_ch);
    }
#if 0
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
  iconv_close(cnv_zip_fn);
  g_free_content:
  iconv_close(cnv_content);
  g_free_jtv:
  FreeChannelAliasList(chl);
  FreeJTV(tvl);
  free(date_format);free(time_format);
  return ret;
}
