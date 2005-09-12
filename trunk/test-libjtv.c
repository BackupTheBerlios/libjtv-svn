#include <stdio.h>
#include <ctype.h>
#include <locale.h>
#include <langinfo.h>
#include <iconv.h>
#include <string.h>
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

  iconv_t cnv = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                           "CP1251");
  if (cnv == (iconv_t) -1)
  {
    printf("Iconv open return %d error code\n", errno);
    return errno;
  }

  tv_list *tvl = LoadJTV(argv[1],"channel.alias.rc");
  unsigned int i;
  char *c_str = new char [512], *c_str_org = c_str;
  int cur_day = -1;
  char *cur_ch = "";

  for (i = 0; i < tvl->num; i++)
  {
    size_t in_buf_len = strlen (tvl->tvp[i].prg_name);
    size_t out_buf_len = 512, cnv_bytes = 0;
    char *in_buf = tvl->tvp[i].prg_name;
    c_str = c_str_org;
    if ((cnv_bytes = iconv(cnv, &in_buf, &in_buf_len, &c_str, &out_buf_len)) == -1)
    {
      printf("Error during conversion.Error message : %s\n",strerror(errno));
      delete [] c_str;

      iconv_close(cnv);
      FreeJTV(tvl);
      return errno;
    }
    c_str_org[512 - out_buf_len] = 0;
    struct tm *tmp;
    tmp = gmtime(&tvl->tvp[i].time);
    if (strcmp(tvl->tvp[i].ch_name, cur_ch) != 0)
    {
	cur_ch = tvl->tvp[i].ch_name;
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
    printf("%s %s\n",
           tbuf,c_str_org);
/*    printf("%s. %02d.%02d.%04d %02d:%02d:%02d %s\n",
           tvl->tvp[i].ch_name,
           tmp->tm_mday, tmp->tm_mon + 1, tmp->tm_year + 1900, tmp->tm_hour, tmp->tm_min, tmp->tm_sec,
           c_str_org);*/

  }

  delete [] c_str_org;

  iconv_close(cnv);
  FreeJTV(tvl);
}
