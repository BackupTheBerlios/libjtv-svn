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

  setlocale(LC_ALL,"");
  char *date_format = strnew(nl_langinfo(D_FMT));
  char *time_format = strnew(nl_langinfo(T_FMT));

  iconv_t cnv1251 = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                               "CP1251");
  if (cnv1251 == (iconv_t) -1)
  {
    printf("Iconv open return %d error code\n", errno);
    return errno;
  }

  iconv_t cnv866 = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                              "CP866");
  if (cnv866 == (iconv_t) -1)
  {
    printf("Iconv open return %d error code\n", errno);
    return errno;
  }

  tv_list *tvl = LoadJTV(argv[1],"channel.alias.rc", cnv866);
  unsigned int i;
  int cur_day = -1;
  char *cur_ch = "";

  for (i = 0; i < tvl->num; i++)
  {
    struct tm *tmp;
    char *pn;
    tmp = gmtime(&tvl->tvp[i].time);
    if (strcmp(tvl->tvp[i].ch_name, cur_ch) != 0)
    {
      cur_ch = tvl->tvp[i].ch_name;
      //printf("Channel %s : ", strnewcnv(cnv866,cur_ch));
      printf("Channel %s : ", cur_ch);
    }

    char tbuf[100];
    if (cur_day != tmp->tm_mday)
    {
      strftime(tbuf,100,date_format,tmp);
      printf("%s\n", tbuf);
      cur_day = tmp->tm_mday;
    }

    strftime(tbuf,100,time_format,tmp);
    pn = strnewcnv(cnv1251, tvl->tvp[i].prg_name);
    printf("%s %s\n",
           tbuf,pn);
    free(pn);
  }
  free(date_format);free(time_format);
  iconv_close(cnv866);
  iconv_close(cnv1251);
  FreeJTV(tvl);
}
