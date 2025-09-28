#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"


static int
startswith(const char *data, const char *prefix)
{
  while(*prefix){
    if(*data++ != *prefix++)
      return 0;
  }
  return 1;
}

int
main(int argc, char *argv[])
{
  const char *marker = "my very very very secret pw is:   ";
  int marker_len = strlen(marker);
  int region = 32 * 4096;
  char *base = sbrk(region);

  if(base == (char*)-1)
    exit(1);

  char *secret = 0;
  for(int i = 0; i <= region - marker_len; i++){
    if(startswith(base + i, marker)){
      secret = base + i + marker_len;
      break;
    }
  }

  if(secret){
    int len = strlen(secret);
    if(len > 0)
      write(2, secret, len);
    exit(0);
  }

  exit(1);
}
