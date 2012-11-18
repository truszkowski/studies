#ifndef __ALTPINFO_STRUCTS_H__
#define __ALTPINFO_STRUCTS_H__

#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/proc_fs.h>
#include <linux/fcntl.h>
#include <linux/seq_file.h>
#include <linux/cdev.h>
#include <linux/mm.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include "altpinfo.h"

#define print_warn(fmt,arg...) printk(KERN_WARNING "altpinfo: " fmt,##arg)
#if (DEBUG_MODE == 1)
	#define print_info(fmt,arg...) printk(KERN_INFO "altpinfo: " fmt,##arg)
	#define print_debug(fmt,arg...) printk(KERN_DEBUG "altpinfo: " fmt,##arg)
#else 
	#define print_info(fmt,arg...) do { } while ( 0 )
	#define print_debug(fmt,arg...) do { } while ( 0 )
#endif

/* mozliwe formaty, */
#define ALTPINFO_FORMAT_NULL     0x00
#define ALTPINFO_FORMAT_PID      0x01
#define ALTPINFO_FORMAT_UID      0x02
#define ALTPINFO_FORMAT_COMM     0x03
#define ALTPINFO_FORMAT_STATE    0x04
#define ALTPINFO_FORMAT_VSIZE    0x05
#define ALTPINFO_FORMAT_SESSION  0x06
#define ALTPINFO_FORMAT_ERROR    0x7f

/* alternatywne zrodlo informacji o procesie, */
struct task_altpinfo {
	int pid, uid, sid;
	unsigned long int vsize;
	char comm[TASK_COMM_LEN+1], state;
};

/* pobranie informacji o procesie, */
extern int get_task_altpinfo( struct task_struct *, struct task_altpinfo * );

/* zarzadanie formatem zapisu informacji, */
typedef char altpinfo_format_t;
struct altpinfo_fmt_t {
	altpinfo_format_t fmt[FMT_CHAR_LIMIT];
	char fmt_charp[FMT_CHAR_LIMIT];
	rwlock_t access;
	int outlen;
};
extern struct altpinfo_fmt_t altpinfo_fmt;

extern int change_fmt( const char * dscr );

struct altpinfo_t {
	char * buffer;                         /* miejsce na informacje, */
	ssize_t buflen;                        /* ile zajmuja, */
	ssize_t bufreallen;                    /* rzeczywista dlugosc bufora */
	loff_t bufpos;                         /* pozycja w buforze, */
	struct semaphore mutex, read, write;   /* semafory... */
};

/* zarzadzanie buforem informacji, */
extern int free_altbuffer( struct altpinfo_t * );
extern int alloc_altbuffer( struct altpinfo_t *, ssize_t );

/* przekopiowanie fmt, by inni mogli spokojnie ioctl-owac, */
#define copy_fmt( cpfmt, size ) do {                                   \
  read_lock( &altpinfo_fmt.access );                                   \
  memcpy( (cpfmt), altpinfo_fmt.fmt, strlen( altpinfo_fmt.fmt ) + 1);  \
  size = altpinfo_fmt.outlen;                                          \
  read_unlock( &altpinfo_fmt.access );                                 \
} while ( 0 ) 

#endif
