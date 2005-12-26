#include <iconv.h>
#include <langinfo.h>
#include <stdio.h>
#include <stdlib.h>
#include "cs/archive.h"
#include "strnew.h"
#include "jtv.h"
#include "libjtv.h"

#define FILETIME_PER_SEC 10000000LL
#define TIME_T_ZERO 0x19DB1F7FA8BB800LL // zerotime(01-01-1970) for FILETIME type
#define HOUR_SEC 3600

char *strnewcnv(iconv_t cnv, char *str)
{
  char *tmp = (char *) malloc(strlen(str) * 3);
  if (tmp != NULL)
  {
    size_t in_buf_len = strlen (str) + 1;
    size_t out_buf_len = in_buf_len * 3, cnv_bytes = 0, old_out_len = out_buf_len;
    char *c_str = str;
    char *o_str = tmp;

    if ((cnv_bytes = iconv(cnv, &c_str, &in_buf_len,
                           &o_str, &out_buf_len)) == 0)
      tmp = (char*)realloc(tmp, old_out_len - out_buf_len);
    else
    {
      free(tmp);
      tmp = NULL;
    }
  }
  return tmp;
}

time_t FileTime2Time_T(unsigned long long ftime, int correctTZ)
{
  //(correctTZ * HOUR_SEC)
  return (time_t) (((ftime - TIME_T_ZERO) / FILETIME_PER_SEC)+(correctTZ * HOUR_SEC));
}

ch_alias_list * LoadChannelAliasList(char *fname)
{
  FILE *in = fopen(fname,"rb");

  if (in != NULL)
  {
    char opt[256],val[256];
    ch_alias_list *chl = (ch_alias_list *) malloc(sizeof(ch_alias_list));
    if (chl)
    {
      chl->num = 0;
      chl->cp_zip_fn = NULL;
      chl->cp_content = NULL;
    }

    while (chl != NULL &&
           fscanf(in, "%250[^= ] = %250[^\n]\n",opt,val) == 2)
    {
      if (strcmp(opt, "cp_zip_fn") == 0)
        chl->cp_zip_fn = strnew(val);
      else
        if (strcmp(opt, "cp_content") == 0)
          chl->cp_content = strnew(val);
        else
        {
          int sn = chl->num;
          chl->num++;

          chl = (ch_alias_list *) realloc(chl,
                                          chl->num * sizeof(ch_alias) +
                                          sizeof(ch_alias_list));
          if (chl)
          {
            chl->cha[sn].zip_name = strnew(opt);
            chl->cha[sn].real_name = strnew(val);
          }
        }
    }
    if (chl->cp_zip_fn == NULL) chl->cp_zip_fn = strnew("CP866");
    if (chl->cp_content == NULL) chl->cp_content = strnew("CP1251");

    fclose(in);
    return chl;
  }

  return NULL;
}

char *GetChannelAlias(ch_alias_list *chl, char *ch_name,
                      int *index)
{
  unsigned int i;

  if (chl != NULL)
    for (i = 0; i < chl->num; i++)
    {
      if (chl->cha[i].zip_name &&
          chl->cha[i].real_name &&
          strcasecmp(chl->cha[i].zip_name, ch_name) == 0)
      {
        if (index) *index = i;
        return chl->cha[i].real_name;
      }
    }

  if (index) *index = -1;
  return ch_name;
}

void FreeChannelAliasList(ch_alias_list *ch_list)
{
  unsigned int i;

  if (ch_list != NULL)
  {
    for (i = 0; i < ch_list->num; i++)
    {
      if (ch_list->cha[i].zip_name)  free(ch_list->cha[i].zip_name);
      if (ch_list->cha[i].real_name) free(ch_list->cha[i].real_name);
    }
    if (ch_list->cp_zip_fn) free(ch_list->cp_zip_fn);
    if (ch_list->cp_content) free(ch_list->cp_content);
    free(ch_list);
  }
}

void FreeJTV(tv_list *tvl)
{
  unsigned int i;
  if (tvl)
  {
    for (i = 0; i < tvl->num; i++)
    {
      free(tvl->tvp[i].ch_name);
      free(tvl->tvp[i].prg_name);
    }

    if (tvl->tvp && tvl->num) free(tvl->tvp);
    free(tvl);
  }
}

void ParseJTV(char *ch_name,
              char *ndx_image, size_t ndx_size,
              char *pdt_image, size_t pdt_size,
              tv_list *tvl,
              int ch_index,
              int correctTZ)
{
  size_t ndx_ptr = sizeof(NDX_HEADER);
  int i = 0;

  // parse ndx file image
  while (ndx_ptr < ndx_size)
  {
    unsigned int sn = tvl->num;
    struct tm tmp;
    NDX_RECORD *ndx_rec = (NDX_RECORD *) (ndx_image + ndx_ptr);

    tvl->num++;
    tvl->tvp = (tv_program*)realloc(tvl->tvp, tvl->num * sizeof(tv_program));
    tvl->tvp[sn].ch_name = strnew(ch_name);
    tvl->tvp[sn].time = FileTime2Time_T(ndx_rec->win_time, correctTZ);
    tvl->tvp[sn].etime = tvl->tvp[sn].time + 1;

    // correct by daylight saving time
    /*gmtime_r(&tvl->tvp[sn].time, &tmp);
    tvl->tvp[sn].tm_isdst = tmp.tm_isdst;
    if (tmp.tm_isdst == 0) tvl->tvp[sn].time = tvl->tvp[sn].time + HOUR_SEC;*/

    tvl->tvp[sn].ch_index = ch_index;

    PDT_RECORD *pdt_rec = (PDT_RECORD *)(pdt_image + ndx_rec->str_seek);
    tvl->tvp[sn].prg_name = (char*)malloc(pdt_rec->sz_str + 1);
    strncpy(tvl->tvp[sn].prg_name, pdt_rec->str, pdt_rec->sz_str);
    tvl->tvp[sn].prg_name[pdt_rec->sz_str] = 0;

    ndx_ptr += sizeof(NDX_RECORD);
    i++;
  }
//  printf("parse %d record\n",i);
}

tv_list *LoadJTV(char *fname, char *ch_alias, int correctTZ)
{
  tv_list *tvl = (tv_list *)malloc(sizeof(tv_list));
  if (!tvl) return NULL;
  tvl->num = 0;
  tvl->tvp = NULL;
  csArchive *jtvFile = new csArchive(fname);

  ch_alias_list *chl = LoadChannelAliasList(ch_alias);
  iconv_t cnv_zip_fn = iconv_open(nl_langinfo(_NL_MESSAGES_CODESET),
                                  chl->cp_zip_fn);
  if (cnv_zip_fn != (iconv_t) -1)
  {
    int i = 0;
    void *ae;
    // while file in vector exist
    while((ae = jtvFile->GetFile(i)) != NULL)
    {
      char *fndx_name = jtvFile->GetFileName(ae);
      //    printf("%d. %s \n",i, fndx_name);
      if (strstr(fndx_name, ".ndx") != NULL)
      {
        //      printf ("processing %s...\n",fndx_name);
        char *fpdt_name = strnew(fndx_name);
        strcpy(strstr(fpdt_name, ".ndx"), ".pdt");
        //      printf("generate %s\n",fpdt_name);

        // 1. check files exist (both ndx and pdt)
        if (jtvFile->FileExists(fndx_name, NULL) &&
            jtvFile->FileExists(fpdt_name, NULL))
        {
          // 2. read files into memory
          size_t ndx_size = 0, pdt_size = 0;
          char *ndx_image = NULL, *pdt_image = NULL;
          if ((ndx_image = jtvFile->Read(fndx_name, &ndx_size)) != NULL &&
              ndx_size != 0)
          {
            if ((pdt_image = jtvFile->Read(fpdt_name, &pdt_size)) != NULL &&
                pdt_size != 0)
            {
              char *ch_name = strnewcnv(cnv_zip_fn, fpdt_name);
              char *c = strstr(ch_name,".pdt");
              *c = 0;
              int ch_index;
              char *alias = GetChannelAlias(chl, ch_name, &ch_index);
              //            printf("Channel name %s \n", ch_name);

              ParseJTV(alias, ndx_image, ndx_size,
                       pdt_image, pdt_size,
                       tvl, ch_index, correctTZ);

              if (ch_name) free(ch_name);
              delete [] pdt_image;
            }

            delete [] ndx_image;
          }

        }

        free(fpdt_name);
      }
      i++;
    }

    if (tvl->num)
      for (i = 0; i< tvl->num - 1; i++)
        if (tvl->tvp[i].ch_index == tvl->tvp[i + 1].ch_index)
          tvl->tvp[i].etime = tvl->tvp[i + 1].time - 1;

    iconv_close(cnv_zip_fn);
  }
  FreeChannelAliasList(chl);
  delete jtvFile;
  return tvl;
}
