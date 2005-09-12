/*
     JTV file format header
     */

#ifdef __GNUC__
#  define PACKED __attribute__((packed))
#else
#  define PACKED
#endif

typedef struct {
  unsigned long long win_time;
  unsigned long str_seek;
} PACKED NDX_RECORD;

typedef struct {
  unsigned long rec_count;
} PACKED NDX_HEADER;

typedef struct {
  unsigned short sz_str;
  char str[0];
} PACKED PDT_RECORD;
