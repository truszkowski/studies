#ifndef __ALTPINFO_H__
#define __ALTPINFO_H__
#include <asm/page.h>
#ifndef PAGE_SIZE
#define PAGE_SIZE 4096
#endif

#define DEBUG_MODE 0
#define ALTPINFO_DEVICE "/dev/altpinfo"
#define API_GET_FMT    1
#define API_SET_FMT    2
#define API_GET_CURLEN 3
#define PAGES_FOR_BUFFER 1
#define BUFLEN PAGE_SIZE*PAGES_FOR_BUFFER
#define WRITE_LIMIT 20
#define FMT_CHAR_LIMIT PAGE_SIZE

#endif
