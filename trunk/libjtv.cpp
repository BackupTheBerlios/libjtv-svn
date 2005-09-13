#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include "cs/archive.h"
#include "strnew.h"
#include "jtv.h"
#include "libjtv.h"

#define TIME_T_ZERO 0x19DB1DED53E8000LL
#define FILETIME_PER_SEC 10000000LL

time_t FileTime2Time_T(unsigned long long ftime)
{
  return (time_t) ((ftime - TIME_T_ZERO) / FILETIME_PER_SEC);
}

ch_alias_list * LoadChannelAliasList(char *fname)
{
  FILE *in = fopen(fname,"rb");

  if (in != NULL)
  {
    char opt[256],val[256];
    ch_alias_list *chl = (ch_alias_list *) malloc(sizeof(ch_alias_list));
    if (chl) chl->num = 0;

    while (chl != NULL &&
           fscanf(in, "%250[^= ] = %250[^\n]\n",opt,val) == 2)
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
    fclose(in);
    return chl;
  }

  return NULL;
}

char *GetChannelAlias(ch_alias_list *chl, char *ch_name, int *index)
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

  if (index) *index = 0;
  return ch_name;
}

void FreeChannelAliasList(ch_alias_list *ch_list)
{
  unsigned int i;

  if (ch_list != NULL)
  {
    for (i = 0; i < ch_list->num; i++)
    {
      if (ch_list->cha[i].zip_name) free(ch_list->cha[i].zip_name);
      if (ch_list->cha[i].real_name) free(ch_list->cha[i].real_name);
    }
    free(ch_list);
  }
}

void FreeJTV(tv_list *tvl)
{
  unsigned int i;
  for (i = 0; i < tvl->num; i++)
  {
    delete [] tvl->tvp[i].ch_name;
    delete [] tvl->tvp[i].prg_name;
  }

  if (tvl->tvp) free(tvl->tvp);
}

void ParseJTV(char *ch_name,
              char *ndx_image, size_t ndx_size,
              char *pdt_image, size_t pdt_size,
              tv_list *tvl,
              int ch_index)
{
  size_t ndx_ptr = sizeof(NDX_HEADER);
  int i = 0;

  // parse ndx file image
  while (ndx_ptr < ndx_size)
  {
    unsigned int sn = tvl->num;
    NDX_RECORD *ndx_rec = (NDX_RECORD *) (ndx_image + ndx_ptr);

    tvl->num++;
    tvl->tvp = (tv_program*)realloc(tvl->tvp, tvl->num * sizeof(tv_program));
    tvl->tvp[sn].ch_name = strnew(ch_name);
    tvl->tvp[sn].time = FileTime2Time_T(ndx_rec->win_time);
    tvl->tvp[sn].ch_index = ch_index;

    PDT_RECORD *pdt_rec = (PDT_RECORD *)(pdt_image + ndx_rec->str_seek);
    tvl->tvp[sn].prg_name = new char[pdt_rec->sz_str + 1];
    strncpy(tvl->tvp[sn].prg_name, pdt_rec->str, pdt_rec->sz_str);
    tvl->tvp[sn].prg_name[pdt_rec->sz_str] = 0;

    ndx_ptr += sizeof(NDX_RECORD);
    i++;
  }
//  printf("parse %d record\n",i);
}

tv_list tvl = { 0, NULL };

tv_list *LoadJTV(char *fname, char *ch_alias)
{
  csArchive *jtvFile = new csArchive(fname);

  ch_alias_list *chl = LoadChannelAliasList(ch_alias);

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
            char *ch_name = strnew(fpdt_name);
            char *c = strstr(ch_name,".pdt");
            *c = 0;
            int ch_index;
            char *alias = GetChannelAlias(chl, ch_name, &ch_index);
//            printf("Channel name %s \n", ch_name);

            ParseJTV(alias, ndx_image, ndx_size, pdt_image, pdt_size, &tvl, ch_index);

            delete [] ch_name;
            delete [] pdt_image;
          }

          delete [] ndx_image;
        }

      }

      delete [] fpdt_name;
    }

    i++;
  }
  delete jtvFile;
  return &tvl;
}
